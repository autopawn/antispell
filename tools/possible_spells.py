import sys

if len(sys.argv) != 4:
    print("usage: python3 %s <characters> <n_min> <n_max>"%sys.argv[0])
    sys.exit(1)

characters = sys.argv[1]
len_min = int(sys.argv[2])
len_max = int(sys.argv[3])

with open("corpus/count_1w100k.txt", "r") as fi:
    for lin in fi:
        cells = lin.strip().split()
        if len(cells) != 2: continue

        word, freq = cells
        word = word.lower()

        if len(word) < len_min: continue
        if len(word) > len_max: continue

        possible = True
        for w in word:
            if w not in characters:
                possible = False
                break
        if possible:
            print(word, freq)
