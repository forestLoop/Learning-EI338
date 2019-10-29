package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

const THRESHOLD = 100
const MAXSIZE = 2000
const MAXINT = 10000

var mux sync.Mutex // for printing only

func show_arr(arr []int, prompt string) {
	mux.Lock() // to avoid interleaving printing
	fmt.Printf("%v: ", prompt)
	for _, x := range arr {
		fmt.Printf("%v ", x)
	}
	fmt.Printf("\n")
	mux.Unlock()
}

func ordinary_sort(arr []int) {
	// selection sort
	n := len(arr)
	for i := 0; i != n; i++ {
		min_pos, min_item := i, arr[i]
		for j := i + 1; j != n; j++ {
			if arr[j] < min_item {
				min_pos, min_item = j, arr[j]
			}
		}
		arr[i], arr[min_pos] = arr[min_pos], arr[i]
	}
}

func merge_sort(arr []int) {
	if len(arr) <= THRESHOLD {
		ordinary_sort(arr)
		return
	}
	n := len(arr)
	wg := sync.WaitGroup{}
	wg.Add(2)
	go func() {
		merge_sort(arr[:n/2])
		wg.Done()
	}()
	go func() {
		merge_sort(arr[n/2:])
		wg.Done()
	}()
	wg.Wait()
	var result []int
	lo1, hi1, lo2, hi2 := 0, n/2, n/2, n
	for lo1 < hi1 && lo2 < hi2 {
		if arr[lo1] < arr[lo2] {
			result = append(result, arr[lo1])
			lo1++
		} else {
			result = append(result, arr[lo2])
			lo2++
		}
	}
	if lo2 < hi2 {
		lo1, hi1 = lo2, hi2
	}
	for lo1 < hi1 {
		result = append(result, arr[lo1])
		lo1++
	}
	for i, _ := range arr {
		arr[i] = result[i]
	}

}

func quick_sort(arr []int) {
	if len(arr) <= THRESHOLD {
		ordinary_sort(arr)
		return
	}
	n := len(arr)
	pivot := rand.Intn(n)
	lo, hi := 0, n-1
	arr[hi], arr[pivot] = arr[pivot], arr[hi]
	for i, _ := range arr {
		if arr[i] < arr[hi] {
			arr[lo], arr[i] = arr[i], arr[lo]
			lo++
		}
	}
	arr[lo], arr[hi] = arr[hi], arr[lo]
	wg := sync.WaitGroup{}
	wg.Add(2)
	go func() {
		quick_sort(arr[:lo])
		wg.Done()
	}()
	go func() {
		quick_sort(arr[lo+1:])
		wg.Done()
	}()
	wg.Wait()
}

func main() {
	rand.Seed(time.Now().UTC().UnixNano())
	arr_size := rand.Intn(MAXSIZE) + 1
	fmt.Printf("Generate a random array of size %v.\n", arr_size)
	var arr, init_arr []int
	for i := 0; i != arr_size; i++ {
		init_arr = append(init_arr, rand.Intn(MAXINT))
	}
	show_arr(init_arr, "Initial")
	arr = append([]int(nil), init_arr...)
	ordinary_sort(arr)
	show_arr(arr, "Selection sort")
	arr = append([]int(nil), init_arr...)
	merge_sort(arr)
	show_arr(arr, "Merge sort")
	arr = append([]int(nil), init_arr...)
	quick_sort(arr)
	show_arr(arr, "Quick sort")
}
