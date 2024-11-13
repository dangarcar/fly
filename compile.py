import os
import subprocess

def main():
    if not os.path.exists('lib/SDL2'):
        print('You must download SDL2 for your system in the lib/ folder')

    if not os.path.exists('release'):
        os.mkdir('release')
    os.chdir('release')
    subprocess.run(['cmake', '-DCMAKE_BUILD_TYPE=Release', '..'])
    subprocess.run('make')
    os.chdir('..')

    if not os.path.exists('saves'):
        os.mkdir('saves')
    with open('saves/worlds.json', 'w') as f:
        f.write('{ "saves": [] }')

if __name__ == '__main__':
    main()