import requests
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-filename", help="enter file name", default="index.html")
args = parser.parse_args()

res = requests.get(f"http://localhost:8080/{args.filename}")

# Print the status, headers, and body
print("Status code:", res.status_code)
print("\nHeaders:")
for key, value in res.headers.items():
    print(f"{key}: {value}")

print("\nBody:")
print(res.text)

# Returns the following:
# Status code: 200

# Headers:
# Content-Type: text/html
# Connection: close

# Body:
# <!DOCTYPE html>
# <html lang="en">
#   <head>
#     <meta charset="UTF-8" />
#     <meta name="viewport" content="width=device-width, initial-scale=1.0" />
#     <title>Document</title>
#   </head>
#   <body>
#     Hell server
#   </body>
# </html>
