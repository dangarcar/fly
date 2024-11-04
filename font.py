import pywavefront

import os

def main():
    with open('./resources/font.fontobj', 'w+') as out:
        for file in os.listdir('./resources/fontdef'):
            path = os.path.join('./resources/font3d', file)
            if not path.endswith('.obj'):
                continue

            with open(path, 'r') as fin, open(os.path.join('./resources/fontdef', file), 'w+') as fout:
                for line in fin:
                    if line.startswith("s") or line.startswith("mtl"):
                        line = ""
                    fout.write(line)

            obj = pywavefront.Wavefront(os.path.join('./resources/fontdef', file), strict=False, collect_faces=True)

            name = input(file + ": ")

            minV = 100000.0
            maxV = 0.0
            for v in obj.vertices:
                minV = min(minV, v[0])
                maxV = max(maxV, v[0])

            out.write(f"\n\no {name}|{round(maxV+minV, 5)}\n")

            for v in obj.vertices:
                out.write(f"v {v[0]} {v[2]}\n")
            for f in obj.mesh_list[0].faces:
                out.write(f"f {f[0]} {f[1]} {f[2]}\n")

if __name__ == "__main__":
    main()