# This script generates the flags
# It must be run after the map.py script

import json
import requests
import subprocess

def main():
    countries = {}
    with open('resources/countries.json', 'r') as c:
        countries = json.load(c)

    flags = {}
    for k in countries:
        code = countries[k]['iso'].lower()
        if k == 'SOL': code = 'somaliland'
        elif k == 'KAS': code = 'pk-jk'
        url = f"https://hatscripts.github.io/circle-flags/flags/{code}.svg"
        response = requests.get(url)
        if response.status_code != 200:
            print("no", countries[k]['name'])
        else:
            with open("a.svg", mode="w") as file:
                file.write(response.text)

            subprocess.run(["convert", "-background", "none", "a.svg", f"assets/countries/{k}.png"])

            flags[k] = response.text
    
    with open('resources/flags.json', 'w', encoding='utf-8') as f:
        json.dump(flags, f)

if __name__ == '__main__':
    main()