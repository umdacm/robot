from flask import Flask
from flask import request
from subprocess import call

app = Flask(__name__)

@app.route('/')
def index():
    return """
<!DOCTYPE html>
<html>
  <head>
    <title>ACM Robot Web Control Panel</title>
  </head>
  <body>
    <h1>ACM Robot Web Control Panel</h1>
    <form method=POST action="/say">
      Say something:
      <input name="text" type="text">
      <input type="submit">
    </form>
  </body>
</html>"""

@app.route("/say", methods=['POST'])
def say():
    call(['ssh', 'mobility@10.0.0.2', 'echo %s >> /dev/ttyS3' % request.form["text"]])
    return "Okay!"
