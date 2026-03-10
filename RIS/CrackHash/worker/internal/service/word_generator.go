package service

func GenWords(startIndex, stopIndex uint64, alphabet []rune, maxLength int) <-chan string {
	out := make(chan string)
	alphabetSize := uint64(len(alphabet))

	go func() {
		defer close(out)

		globalOffset := uint64(0)

		// for each block of words with same length
		for wordLength := 1; wordLength <= maxLength; wordLength++ {
			blockSize := UInt64Pow(alphabetSize, uint64(wordLength))
			blockStart := globalOffset
			blockStop := globalOffset + blockSize

			// not yet reached index range -> continue
			if startIndex >= blockStop {
				globalOffset += blockSize
				continue
			}

			// passed the end of index range -> finish
			if stopIndex <= blockStart {
				break
			}

			localStart := max(startIndex, blockStart) - blockStart
			localStop := min(stopIndex, blockStop) - blockStart

			genWordsInBlock(localStart, localStop, alphabet, blockSize, wordLength, out)

			globalOffset += blockSize
		}
	}()

	return out
}

func genWordsInBlock(
	localStart, localStop uint64,
	alphabet []rune,
	blockSize uint64,
	wordLength int,
	out chan<- string,
) {
	alphabetSize := uint64(len(alphabet))

	if localStart >= blockSize || localStop > blockSize {
		panic("index range does not fit into block")
	}

	word := make([]rune, wordLength)

	for index := localStart; index < localStop; index++ {
		temp := index
		for i := range wordLength {
			word[i] = alphabet[temp%alphabetSize]
			temp /= alphabetSize
		}

		out <- string(word)
	}
}

