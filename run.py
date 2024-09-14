import os

def main():
    if not os.path.exists('build'):
        os.makedirs('build')
    os.chdir('build')
    os.system('cmake .. -G "MinGW Makefiles"')
    os.system('make')
    os.chdir('..')
    os.startfile('build\Fly.exe')

if __name__ == "__main__":
    main()