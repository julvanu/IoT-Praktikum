## Setup

### Create python venv
`python -m venv venv`

### Activate venv

| console    | command                               |
|------------|---------------------------------------|
| cmd.exe    | `C:\> <venv>\Scripts\activate.bat`    |
| PowerShell | `PS C:\> <venv>\Scripts\Activate.ps1` |
| bash       | `source <venv>/bin/activate`          |

### Install requirements
`pip install -r requirements.txt`

To initially create or update requirements.txt (with specific versions) from current setup: <br>
`pip freeze > requirements.txt`