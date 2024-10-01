import os
import subprocess

def main():
    if not os.path.exists('release'):
        os.mkdir('release')
    os.chdir('release')
    subprocess.run(['cmake', '-DCMAKE_BUILD_TYPE=Release', '..'])
    subprocess.run('make')
    os.chdir('..')
    subprocess.run('release/Fly.exe')

if __name__ == '__main__':
    main()