from influxdb_client import InfluxDBClient
import pandas as pd
from datetime import datetime, timedelta
from zoneinfo import ZoneInfo

from influxdb_client.client.write_api import SYNCHRONOUS

# ============================================================
# 1. CONFIGURATION
# ============================================================
INFLUX_URL = "http://100.107.165.205:8086/"
INFLUX_TOKEN = "SBto4EBQvq7wY-APYOoDn4QpYZ9GkjWzQZzftrDDk31kjRYWYN-37i7uHNXddkZjYsAU85EdYbI2hoKBLB1woA=="
INFLUX_ORG = "e35fd59963c483cd"
INFLUX_BUCKET_CORRIDOR = "1_1_1"
INFLUX_BUCKET_BATHROOM = "1_2_6"
INFLUX_BUCKET_KITCHEN = "1_4_10"
INFLUX_BUCKET_LIVINGROOM = "1_5_11"
INFLUX_BUCKET_BEDROOM = "1_6_12"
INFLUX_BUCKET_AUTH = ""

CONFIG_STAY_MAX_INTERVAL = 10


def get_bucket_from_device(device_id: int):
    if device_id == 1:
        bucket = INFLUX_BUCKET_CORRIDOR
    elif device_id == 2:
        bucket = INFLUX_BUCKET_BATHROOM
    elif device_id == 4:
        bucket = INFLUX_BUCKET_KITCHEN
    elif device_id == 5:
        bucket = INFLUX_BUCKET_LIVINGROOM
    elif device_id == 6:
        bucket = INFLUX_BUCKET_BEDROOM
    else:
        return None
    return bucket


# ============================================================
# 2. CONNECT TO INFLUXDB
# ============================================================
def get_client():
    return InfluxDBClient(
        url=INFLUX_URL,
        token=INFLUX_TOKEN,
        org=INFLUX_ORG
    )


# ============================================================
# 3. QUERY DATA FROM INFLUXDB
# ============================================================
def query_all_device_data(start: str, stop: str):
    df = pd.DataFrame()
    for device_id in [1, 2, 4, 5, 6]:
        df_temp = query_data(device_id=device_id, start=start, stop=stop)
        df = pd.concat([df, df_temp], ignore_index=True)
    return df

def query_data(device_id=1, start=None, stop=None, range_in_hours=None):
    query_api = client.query_api()

    bucket = get_bucket_from_device(device_id)
    if bucket is None:
        print("Unknown device ID.")
        return

    if start is not None and stop is not None:
        query = f'''
               from(bucket: "{bucket}")
                 |> range(start: {start}, stop: {stop})
                 |> filter(fn: (r) => r["_measurement"] == "PIR")
                 |> keep(columns: ["_time", "_value", "_field", "_measurement"])
            |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
           '''
    ###### |> pivot(rowKey: ["_time"], columnKey: ["_field"], valueColumn: "_value") ############
    elif range_in_hours is not None:
        query = f'''
            from(bucket: "{bucket}")
              |> range(start: -{range_in_hours}h)
              |> filter(fn: (r) => r["_measurement"] == "PIR")
              |> keep(columns: ["_time", "_value", "_field", "_measurement"])
            |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
        '''
    else:
        print("You need to provide either start and stop time or a range in hours.")
        return pd.DataFrame({})

    print("Running query...")
    df = query_api.query_data_frame(query)

    # If Influx returns multiple tables, flatten them
    if isinstance(df, list):
        df = pd.concat(df, ignore_index=True)

    print(f"\nFound {df.shape[0]} rows of data for device {device_id}")
    return df


# ============================================================
# 4. PROCESS DATA
# ============================================================
def add_to_df(df, start_time: pd.Timestamp, end_time: pd.Timestamp, room):
    duration = pd.Timedelta(seconds=0) if end_time is None else end_time - start_time
    print(f"Duration: {duration.seconds}")
    df.loc[len(df)] = [room, start_time, duration.seconds]


def get_previous_room(query_end: pd.Timestamp):
    print("Querying previous room...")
    query_api = client.query_api()
    bucket = "stays"
    query_end = query_end.to_pydatetime()  # type: datetime
    query_start = query_end - timedelta(days=1)
    start = query_start.strftime('%Y-%m-%dT%H:%M:%SZ')
    stop = query_end.strftime('%Y-%m-%dT%H:%M:%SZ')

    print(f"Start: {start}, stop: {stop}")

    query = f'''
        from(bucket: "{bucket}")
          |> range(start: {start}, stop: {stop})
          |> filter(fn: (r) => r["_measurement"] == "room_stays")
          |> max(column: "_time")
          |> keep(columns: ["_time", "room", "_value"])
    '''
    df_result = query_api.query_data_frame(query)
    df_result = df_result.sort_values(by="_time", ascending=False)
    print(df_result)
    room = df_result.at[0, "room"]
    print(room)
    return room


# Returns the row of the next event after the given time query_start
# Does not check, however, whether that event was triggered by a different device than the previous one
def get_next_event(query_start: pd.Timestamp):
    print("Querying previous room...")
    query_api = client.query_api()
    bucket = "stays"
    query_start = query_start.to_pydatetime()  # type: datetime
    query_end = query_start + timedelta(days=1)
    start = query_start.strftime('%Y-%m-%dT%H:%M:%SZ')
    stop = query_end.strftime('%Y-%m-%dT%H:%M:%SZ')

    df = query_all_device_data(start, stop)
    if df.empty:
        return None
    row = df.loc[df["_time"].idxmin()]
    return row


def analyze_stays(df: pd.DataFrame, query_start: pd.Timestamp, query_stop: pd.Timestamp) -> pd.DataFrame:
    print("\nAnalyzing stays...")
    if df.empty:
        print("\nEmpty dataframe.")
        return df

    df_stays = pd.DataFrame({
        "room": [],
        "start_time": [],
        "duration": []
    })

    df_sorted = df.sort_values(by="_time")  # ascending by default

    start_time = df_sorted.iloc[0]["_time"]
    room = df_sorted.iloc[0]["roomID"]
    # start_time = query_start  # type: pd.Timestamp
    # room = get_previous_room(query_start)

    for idx, row in df_sorted.iterrows():
        if row['roomID'] != room:
            print(f"Room changed from {room} to {row['roomID']} at time {row['_time']}")
            # Write previous stay to df
            add_to_df(df_stays, start_time, row['_time'], room)
            # Reset start_time & room
            start_time = row['_time']
            room = row['roomID']

    # Add stay for last room until end of query window
    row = get_next_event(query_stop)
    if row is None:
        add_to_df(df_stays, start_time, query_stop, room)
    else:
        add_to_df(df_stays, start_time, row['_time'], row['roomID'])

    return df_stays


# ============================================================
# 5. WRITE STAYS AND ACTIVITIES: BACK TO INFLUXDB
# ============================================================
def write_results(df_stays: pd.DataFrame):
    print("\nWriting results to InfluxDB...")

    # Convert start_time to datetime (required by InfluxDB)
    df_stays["start_time"] = pd.to_datetime(df_stays["start_time"])
    # Set the timestamp column as index — InfluxDB uses the index as time
    df_stays = df_stays.set_index("start_time")

    write_api = client.write_api(write_options=SYNCHRONOUS)

    print("\nData is ready to be written into DB:")
    print(df_stays)

    # Write DataFrame to InfluxDB
    write_api.write(
        bucket="stays",
        record=df_stays,
        data_frame_measurement_name="room_stays",  # measurement name
        data_frame_tag_columns=["room"]  # "room" becomes a tag
    )

    print("Data written successfully to bucket 'stays'")


# ============================================================
# 6. MAIN PIPELINE
# ============================================================
if __name__ == "__main__":
    print("Connecting to InfluxDB...")
    client = get_client()

    # start_date = "2025-12-01"
    # stop_date = "2025-12-19"
    stop_date = datetime.now(ZoneInfo("Europe/Berlin")).date() - timedelta(days=1)
    start_date = stop_date - timedelta(days=1)

    start = f"{start_date}T00:00:00Z"
    stop = f"{stop_date}T00:00:00Z"
    print(f"Querying data for period from {start} until {stop}...")

    df = query_all_device_data(start, stop)

    df_stays = analyze_stays(df, query_start=pd.Timestamp(start), query_stop=pd.Timestamp(stop))
    write_results(df_stays)
    print(df_stays)

    print("\nPipeline finished.")
