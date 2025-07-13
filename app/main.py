from flask import Flask

app = Flask(__name__)

@app.route("/")
def home():
    return "Test flask app"

if __name__ == "__main__":
    app.run(debug=True)