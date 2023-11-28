from flask import Flask, render_template, request, abort, jsonify
from datetime import datetime, timedelta
import json

app = Flask(__name__)

# Route to serve weather data as JSON
@app.route('/weather_data')
def weather_data():
    try:
        with open('pc_weather_data.json', 'r') as file:
            data = json.load(file)
            dates = [(datetime.now() + timedelta(days=i)).strftime('%Y-%m-%d') for i in range(5)]
            highs = [item['h'] for item in data]
            lows = [item['l'] for item in data]
            rain_chance = [item['r'] for item in data]
            return jsonify({'dates': dates, 'highs': highs, 'lows': lows, 'rain_chance': rain_chance})
    except FileNotFoundError:
        abort(500, 'Weather data file not found')

@app.route('/')
def index():
    city = request.args.get('city')
    print(city)
    
    if city is None:
        abort(400, 'Missing argument city')
    
    # Save the value of city to a txt file
    with open('city_value.txt', 'w') as txt_file:
        txt_file.write(str(city))
    
    return render_template('index.html', title='Weather App')

if __name__ == '__main__':
    app.run(debug=True)
