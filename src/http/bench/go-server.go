// Go HTTP benchmark server for comparison with Icey.
// Usage: go run go-server.go [minimal|echo]
package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
)

const port = ":1337"

func main() {
	mode := "minimal"
	if len(os.Args) > 1 {
		mode = os.Args[1]
	}

	switch mode {
	case "minimal":
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			w.Header().Set("Content-Length", "0")
			w.Header().Set("Connection", "close")
			w.WriteHeader(200)
		})
		fmt.Printf("Go HTTP benchmark (minimal) listening on port%s\n", port)

	case "keepalive":
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			w.Header().Set("Content-Length", "0")
			w.WriteHeader(200)
		})
		fmt.Printf("Go HTTP benchmark (keep-alive) listening on port%s\n", port)

	case "echo":
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			io.Copy(w, r.Body)
		})
		fmt.Printf("Go HTTP echo listening on port%s\n", port)

	default:
		fmt.Fprintf(os.Stderr, "Usage: go run go-server.go [minimal|echo]\n")
		os.Exit(1)
	}

	if err := http.ListenAndServe(port, nil); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
}
