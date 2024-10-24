package main

import (
	"bufio"
	"encoding/json"
	"sync"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"strconv"

	"github.com/joho/godotenv"
)

type GeocodingResponse struct {
	Hits []struct {
		Point struct {
			Lat float64 `json:"lat"`
			Lng float64 `json:"lng"`
		} `json:"point"`
		Extent      []float64 `json:"extent"`
		Name        string    `json:"name"`
		Country     string    `json:"country"`
		Countrycode string    `json:"countrycode"`
		City        string    `json:"city"`
		State       string    `json:"state"`
		Postcode    string    `json:"postcode"`
		OsmID       int       `json:"osm_id"`
		OsmType     string    `json:"osm_type"`
		OsmKey      string    `json:"osm_key"`
		OsmValue    string    `json:"osm_value"`
	} `json:"hits"`
	Locale string `json:"locale"`
}

func geocodingRequest(placeName string) GeocodingResponse {
	reqUrl := "https://graphhopper.com/api/1/geocode"
	req, err := http.NewRequest("GET", reqUrl, nil)

	query := req.URL.Query()
	query.Add("q", placeName)
	query.Add("locale", "ru")
	query.Add("key", os.Getenv("GRAPHHOPPER_API_KEY"))
	req.URL.RawQuery = query.Encode()

	if err != nil {
		panic(err)
	}
	res, err := http.DefaultClient.Do(req)
	if err != nil {
		panic(err)
	}
	defer res.Body.Close()
	body, err := io.ReadAll(res.Body)
	if err != nil {
		panic(err)
	}

	var response GeocodingResponse
	if err := json.Unmarshal(body, &response); err != nil {
		fmt.Println("Can not unmarshal JSON")
	}

	return response
}

type WeatherResponse struct {
	Coord struct {
		Lon float64 `json:"lon"`
		Lat float64 `json:"lat"`
	} `json:"coord"`
	Weather []struct {
		ID          int    `json:"id"`
		Main        string `json:"main"`
		Description string `json:"description"`
		Icon        string `json:"icon"`
	} `json:"weather"`
	Base string `json:"base"`
	Main struct {
		Temp      float64 `json:"temp"`
		FeelsLike float64 `json:"feels_like"`
		TempMin   float64 `json:"temp_min"`
		TempMax   float64 `json:"temp_max"`
		Pressure  int     `json:"pressure"`
		Humidity  int     `json:"humidity"`
		SeaLevel  int     `json:"sea_level"`
		GrndLevel int     `json:"grnd_level"`
	} `json:"main"`
	Visibility int `json:"visibility"`
	Wind       struct {
		Speed float64 `json:"speed"`
		Deg   int     `json:"deg"`
		Gust  float64 `json:"gust"`
	} `json:"wind"`
	Clouds struct {
		All int `json:"all"`
	} `json:"clouds"`
	Dt  int `json:"dt"`
	Sys struct {
		Country string `json:"country"`
		Sunrise int    `json:"sunrise"`
		Sunset  int    `json:"sunset"`
	} `json:"sys"`
	Timezone int    `json:"timezone"`
	ID       int    `json:"id"`
	Name     string `json:"name"`
	Cod      int    `json:"cod"`
}

func weatherRequest(lat, lng float64) WeatherResponse {
	reqUrl := "https://api.openweathermap.org/data/2.5/weather"
	req, err := http.NewRequest("GET", reqUrl, nil)

	query := req.URL.Query()
	query.Add("lat", strconv.FormatFloat(lat, 'f', -1, 64))
	query.Add("lon", strconv.FormatFloat(lng, 'f', -1, 64))
	query.Add("units", "metric")
	query.Add("appid", os.Getenv("OPENWEATHERMAP_API_KEY"))
	req.URL.RawQuery = query.Encode()

	if err != nil {
		panic(err)
	}
	res, err := http.DefaultClient.Do(req)
	if err != nil {
		panic(err)
	}
	defer res.Body.Close()
	body, err := io.ReadAll(res.Body)
	if err != nil {
		panic(err)
	}

	var response WeatherResponse
	if err := json.Unmarshal(body, &response); err != nil {
		fmt.Println("Can not unmarshal JSON")
	}

	return response
}

func weatherPrettyPrint(dto *WeatherResponse) {
	fmt.Printf("Weather at %s:\n", dto.Name)
	for i := 0; i < len(dto.Weather); i++ {
		fmt.Printf("\t%s: %s\n", dto.Weather[i].Main, dto.Weather[i].Description)
	}
	fmt.Printf("\tTemp: %f C\n", dto.Main.Temp)
	fmt.Printf("\tFeels like: %f C\n", dto.Main.FeelsLike)
	fmt.Printf("\tHumidity: %d%%\n", dto.Main.Humidity)
}

type InterestingPlacesResponse struct {
	Type     string `json:"type"`
	Features []struct {
		Type     string `json:"type"`
		ID       string `json:"id"`
		Geometry struct {
			Type        string    `json:"type"`
			Coordinates []float64 `json:"coordinates"`
		} `json:"geometry"`
		Properties struct {
			Xid   string  `json:"xid"`
			Name  string  `json:"name"`
			Dist  float64 `json:"dist"`
			Rate  int     `json:"rate"`
			Osm   string  `json:"osm"`
			Kinds string  `json:"kinds"`
		} `json:"properties"`
	} `json:"features"`
}

func interestingPlacesRequest(lat, lng float64) InterestingPlacesResponse {
	reqUrl := "https://api.opentripmap.com/0.1/ru/places/radius"
	req, err := http.NewRequest("GET", reqUrl, nil)

	query := req.URL.Query()
	query.Add("lat", strconv.FormatFloat(lat, 'f', -1, 64))
	query.Add("lon", strconv.FormatFloat(lng, 'f', -1, 64))
	query.Add("radius", "5000")
	query.Add("limit", "5")
	query.Add("apikey", os.Getenv("OPENTRIPMAP_API_KEY"))
	req.URL.RawQuery = query.Encode()

	if err != nil {
		panic(err)
	}
	res, err := http.DefaultClient.Do(req)
	if err != nil {
		panic(err)
	}
	defer res.Body.Close()
	body, err := io.ReadAll(res.Body)
	if err != nil {
		panic(err)
	}

	fmt.Println(res)
	fmt.Println(string(body))

	var response InterestingPlacesResponse
	if err := json.Unmarshal(body, &response); err != nil {
		fmt.Println("Can not unmarshal JSON")
	}

	return response
}

type InterestingPlaceDescriptionResponse struct {
	Xid     string `json:"xid"`
	Name    string `json:"name"`
	Address struct {
		Road          string `json:"road"`
		Town          string `json:"town"`
		State         string `json:"state"`
		County        string `json:"county"`
		Suburb        string `json:"suburb"`
		Country       string `json:"country"`
		Postcode      string `json:"postcode"`
		CountryCode   string `json:"country_code"`
		Neighbourhood string `json:"neighbourhood"`
	} `json:"address"`
	Rate    string `json:"rate"`
	Osm     string `json:"osm"`
	Kinds   string `json:"kinds"`
	Sources struct {
		Geometry   string   `json:"geometry"`
		Attributes []string `json:"attributes"`
	} `json:"sources"`
	Otm   string `json:"otm"`
	Point struct {
		Lon float64 `json:"lon"`
		Lat float64 `json:"lat"`
	} `json:"point"`
}

func interestingPlaceDescriptionRequest(xid string) InterestingPlaceDescriptionResponse {
	reqUrl := fmt.Sprintf("https://api.opentripmap.com/0.1/ru/places/xid/%s", xid)
	req, err := http.NewRequest("GET", reqUrl, nil)

	query := req.URL.Query()
	query.Add("apikey", os.Getenv("OPENTRIPMAP_API_KEY"))
	req.URL.RawQuery = query.Encode()

	if err != nil {
		panic(err)
	}
	res, err := http.DefaultClient.Do(req)
	if err != nil {
		panic(err)
	}
	defer res.Body.Close()
	body, err := io.ReadAll(res.Body)
	if err != nil {
		panic(err)
	}

	fmt.Println(res)
	fmt.Println(string(body))

	var response InterestingPlaceDescriptionResponse
	if err := json.Unmarshal(body, &response); err != nil {
		fmt.Println("Can not unmarshal JSON")
	}

	return response
}

func printPlaceDescription(descr *InterestingPlaceDescriptionResponse) {
    fmt.Printf("%s\n", descr.Name)
}

type LocationData struct {
    weather *WeatherResponse
    interestingPlaces []InterestingPlaceDescriptionResponse
}

func collectLocationData(lat, lng float64) LocationData {
    var data LocationData

    weatherChan := make(chan WeatherResponse, 1)
    go func() {
        weatherChan <- weatherRequest(lat, lng)
    }()

    interestingPlacesChan := make(chan InterestingPlacesResponse, 1)
    go func() {
        interestingPlacesChan <- interestingPlacesRequest(lat, lng)
    }()

    interestingPlaces := <-interestingPlacesChan

    var descrWG sync.WaitGroup
    descrChan := make (chan InterestingPlaceDescriptionResponse)
    for _, place := range interestingPlaces.Features {
        descrWG.Add(1)
        go func() {
            defer descrWG.Done()
            descrChan <- interestingPlaceDescriptionRequest(place.Properties.Xid)
        }()
    }

    go func() {
		descrWG.Wait()
		close(descrChan)
	}()

    data.interestingPlaces = make([]InterestingPlaceDescriptionResponse, 0)
    for descr := range descrChan {
        data.interestingPlaces = append(data.interestingPlaces, descr)
    }

    *data.weather = <-weatherChan

    return data
}

func choosePlace(variants *GeocodingResponse) int {
	fmt.Println("Choose interested place:")

	for i := 0; i < len(variants.Hits); i++ {
		fmt.Printf("[%d]: %s, %s, %s, %s, %s\n",
			i,
			variants.Hits[i].Name,
			variants.Hits[i].City,
			variants.Hits[i].State,
			variants.Hits[i].Country,
			variants.Hits[i].Postcode,
		)
	}

	var num int
	fmt.Print("Enter number: ")
	fmt.Scanln(&num)

	return num
}

func main() {
    fmt.Printf("pid: %d\n", os.Getpid())

	err := godotenv.Load()
	if err != nil {
		log.Fatal("Error loading .env file")
	}

	reader := bufio.NewReader(os.Stdin)

	fmt.Print("Enter place name: ")
	placeName, _ := reader.ReadString('\n')

	geocodingResponse := geocodingRequest(placeName)

	placeNum := choosePlace(&geocodingResponse)

	lat := geocodingResponse.Hits[placeNum].Point.Lat
	lng := geocodingResponse.Hits[placeNum].Point.Lng

    locationData := collectLocationData(lat, lng)

	weatherPrettyPrint(locationData.weather)
    for _, descr := range locationData.interestingPlaces {
        printPlaceDescription(&descr)
    }
}
