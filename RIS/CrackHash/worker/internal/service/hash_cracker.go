package service

import (
	"crypto/md5"
	"encoding/hex"
)

func ComputeHash(word string) string {
	hash := md5.Sum([]byte(word))
	return hex.EncodeToString(hash[:])
}

func CrackHash(targetHash string, words <-chan string) []string {
	var results []string

	for word := range words {
		if ComputeHash(word) == targetHash {
			results = append(results, word)
		}
	}

	return results
}
