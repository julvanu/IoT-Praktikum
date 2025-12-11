"""
Complete InfluxDB 2.x ETL Example Script
----------------------------------------
This script demonstrates:
  - Connecting to InfluxDB
  - Querying data
  - Converting to pandas
  - Running analysis
  - Writing results back to InfluxDB
"""

from influxdb_client import InfluxDBClient
import pandas as pd
from datetime import timedelta

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
INFLUX_BUCKET_AUTH = ""

CONFIG_STAY_MAX_INTERVAL = 10


def get_bucket_from_device(device_id: int):
    if device_id == 1:
        bucket = INFLUX_BUCKET_CORRIDOR
    elif device_id == 2:
        bucket = INFLUX_BUCKET_BATHROOM
    elif device_id == 4:
        bucket = INFLUX_BUCKET_KITCHEN
    else:
        print("Unknown device ID.")
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
def query_data(device_id=1,  start=None, stop=None, range_in_hours=None):
    query_api = client.query_api()
    MEASUREMENT_PIR = "PIR"

    if device_id == 1:
        bucket = INFLUX_BUCKET_CORRIDOR
    elif device_id == 2:
        bucket = INFLUX_BUCKET_BATHROOM
    elif device_id == 4:
        bucket = INFLUX_BUCKET_KITCHEN
    else:
        print("Unknown device ID.")
        return

    if start is not None and stop is not None:
        query = f'''
               from(bucket: "{bucket}")
                 |> range(start: {start}, stop: {stop})
                 |> filter(fn: (r) => r["_measurement"] == "{MEASUREMENT_PIR}")
                 |> keep(columns: ["_time", "_value", "_field", "_measurement"])
           '''
       ###### |> pivot(rowKey: ["_time"], columnKey: ["_field"], valueColumn: "_value") ############
    elif range_in_hours is not None:
        query = f'''
            from(bucket: "{bucket}")
              |> range(start: -{range_in_hours}h)
              |> filter(fn: (r) => r["_measurement"] == "{MEASUREMENT_PIR}")
              |> keep(columns: ["_time", "_value", "_field", "_measurement"])
        '''
    else:
        print("You need to provide either start and stop time or a range in hours.")
        return pd.DataFrame({})

    print("Running query...")
    df = query_api.query_data_frame(query)

    # If Influx returns multiple tables, flatten them
    if isinstance(df, list):
        df = pd.concat(df, ignore_index=True)

    print(f"Retrieved {len(df)} rows.")
    return df


# ============================================================
# 4. PROCESS DATA
# ============================================================
def add_to_df(df, start_time, latest_time, room):
    duration = pd.Timedelta(seconds=0) if latest_time is None else latest_time - start_time
    print(f"Duration: {duration.seconds}")
    df.loc[len(df)] = [room, start_time, duration.seconds]


def analyze_stays(df: pd.DataFrame):
    print("\nAnalyzing stays...")
    if df.empty:
        print("\nEmpty dataframe.")
        return df

    room = df.loc[0, '_value']

    df_stays = pd.DataFrame({
        "room": [],
        "start_time": [],
        "duration": []
    })

    df.sort_values(by="_time")  # ascending by default
    time_array = df["_time"]
    start_time = df.loc[0, '_time']  # type: pd.Timestamp
    print(f"Start time: {start_time}")

    print(f"Type of start_time_str: {type(start_time)}")
    latest_time = None
    for idx, event_time in enumerate(time_array[1:]):
        time_passed = event_time - start_time
        # Check if event is part of the same stay
        if time_passed < timedelta(minutes=CONFIG_STAY_MAX_INTERVAL):
            # If part of stay, extend latest_time to current event_time and continue with next event
            latest_time = event_time
            continue
        else:
            # If not, write stay to df and set current event_time as new start_time
            add_to_df(df_stays, start_time, latest_time, room)
            # Reset start_time & latest_time
            latest_time = None
            start_time = event_time

    if start_time is not None:
        add_to_df(df_stays, start_time, latest_time, room)
    return df_stays


# ============================================================
# 5. WRITE RESULTS BACK TO INFLUXDB
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

    device_id = 1
    # range_in_hours = 6
    start = "2025-12-01T00:00:00Z"
    stop = "2025-12-08T23:59:59Z"

    df = query_data(device_id=device_id, start=start, stop=stop)
    print(f"\nFound {df.shape[0]} rows of data")
    df_stays = analyze_stays(df)
    write_results(df_stays)

    print("\nPipeline finished.")
