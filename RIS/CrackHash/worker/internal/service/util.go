package service

func EnsureSlice[T any](s []T) []T {
	if s == nil {
		return []T{}
	}
	return s
}

func TotalWordCount(maxWordLength, alphabetLength uint64) uint64 {
	wordCount := uint64(0)
	for l := uint64(1); l <= maxWordLength; l++ {
		wordCount += UInt64Pow(alphabetLength, l)
	}

	return wordCount
}

func UInt64Pow(n, m uint64) uint64 {
	if m == 0 {
		return 1
	}

	if m == 1 {
		return n
	}

	result := n
	for i := uint64(2); i <= m; i++ {
		result *= n
	}
	return result
}
