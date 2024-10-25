package main

import (
	"bufio"
	"encoding/json"
	"errors"
	"time"
	"fmt"
	"log"
	"net/http"
	"os"
	"strconv"
	"sync"

	"github.com/joho/godotenv"
    "github.com/briandowns/spinner"
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

func geocodingRequest(placeName string) (*GeocodingResponse, error) {
	reqUrl := "https://graphhopper.com/api/1/geocode"
	req, err := http.NewRequest("GET", reqUrl, nil)
	if err != nil {
		return nil, err
	}

	query := req.URL.Query()
	query.Add("q", placeName)
	query.Add("locale", "ru")
	query.Add("key", os.Getenv("GRAPHHOPPER_API_KEY"))
	req.URL.RawQuery = query.Encode()

	res, err := http.DefaultClient.Do(req)
	if err != nil {
		return nil, err
	}

	defer res.Body.Close()

	if res.StatusCode != 200 {
		return nil, errors.New(res.Status)
	}

	var response GeocodingResponse
	err = json.NewDecoder(res.Body).Decode(&response)
	if err != nil {
		return nil, err
	}

	return &response, nil
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

func weatherRequest(lat, lng float64) (*WeatherResponse, error) {
	reqUrl := "https://api.openweathermap.org/data/2.5/weather"
	req, err := http.NewRequest("GET", reqUrl, nil)
	if err != nil {
		return nil, err
	}

	query := req.URL.Query()
	query.Add("lat", strconv.FormatFloat(lat, 'f', -1, 64))
	query.Add("lon", strconv.FormatFloat(lng, 'f', -1, 64))
	query.Add("units", "metric")
	query.Add("appid", os.Getenv("OPENWEATHERMAP_API_KEY"))
	req.URL.RawQuery = query.Encode()

	res, err := http.DefaultClient.Do(req)
	if err != nil {
		return nil, err
	}

	defer res.Body.Close()

	if res.StatusCode != 200 {
		return nil, errors.New(res.Status)
	}

	var response WeatherResponse
	err = json.NewDecoder(res.Body).Decode(&response)
	if err != nil {
		return nil, err
	}

	return &response, nil
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
	Count    int `json:"count"`
	Next     any `json:"next"`
	Previous any `json:"previous"`
	Results  []struct {
		ID            int    `json:"id"`
		Title         string `json:"title"`
		Slug          string `json:"slug"`
		Address       string `json:"address"`
		Phone         string `json:"phone"`
		SiteURL       string `json:"site_url"`
		Subway        string `json:"subway"`
		IsClosed      bool   `json:"is_closed"`
		Location      string `json:"location"`
		HasParkingLot bool   `json:"has_parking_lot"`
	} `json:"results"`
}

func interestingPlacesRequest(lat, lng float64) (*InterestingPlacesResponse, error) {
	reqUrl := "https://kudago.com/public-api/v1.4/places"
	req, err := http.NewRequest("GET", reqUrl, nil)
	if err != nil {
		return nil, err
	}

	query := req.URL.Query()
	query.Add("lat", strconv.FormatFloat(lat, 'f', -1, 64))
	query.Add("lon", strconv.FormatFloat(lng, 'f', -1, 64))
	query.Add("radius", "10000")
	query.Add("page_size", "5")
	req.URL.RawQuery = query.Encode()

	res, err := http.DefaultClient.Do(req)
	if err != nil {
		return nil, err
	}

	defer res.Body.Close()

	if res.StatusCode != 200 {
		return nil, errors.New(res.Status)
	}

	var response InterestingPlacesResponse
	err = json.NewDecoder(res.Body).Decode(&response)
	if err != nil {
		return nil, err
	}

	return &response, nil
}

type InterestingPlaceDescriptionResponse struct {
	ID          int    `json:"id"`
	Title       string `json:"title"`
	Slug        string `json:"slug"`
	Address     string `json:"address"`
	Timetable   string `json:"timetable"`
	Phone       string `json:"phone"`
	IsStub      bool   `json:"is_stub"`
	BodyText    string `json:"body_text"`
	Description string `json:"description"`
	SiteURL     string `json:"site_url"`
	ForeignURL  string `json:"foreign_url"`
	Coords      struct {
		Lat float64 `json:"lat"`
		Lon float64 `json:"lon"`
	} `json:"coords"`
	Subway         string `json:"subway"`
	FavoritesCount int    `json:"favorites_count"`
	Images         []struct {
		Image  string `json:"image"`
		Source struct {
			Name string `json:"name"`
			Link string `json:"link"`
		} `json:"source"`
	} `json:"images"`
	CommentsCount   int      `json:"comments_count"`
	IsClosed        bool     `json:"is_closed"`
	Categories      []string `json:"categories"`
	ShortTitle      string   `json:"short_title"`
	Tags            []string `json:"tags"`
	Location        string   `json:"location"`
	AgeRestriction  any      `json:"age_restriction"`
	DisableComments bool     `json:"disable_comments"`
	HasParkingLot   bool     `json:"has_parking_lot"`
}

func interestingPlaceDescriptionRequest(id int) (*InterestingPlaceDescriptionResponse, error) {
	reqUrl := fmt.Sprintf("https://kudago.com/public-api/v1.4/places/%d", id)
	req, err := http.NewRequest("GET", reqUrl, nil)
	if err != nil {
		return nil, err
	}

	query := req.URL.Query()
	req.URL.RawQuery = query.Encode()

	res, err := http.DefaultClient.Do(req)
	if err != nil {
		return nil, err
	}

	defer res.Body.Close()

	if res.StatusCode != 200 {
		return nil, errors.New(res.Status)
	}

	var response InterestingPlaceDescriptionResponse
	err = json.NewDecoder(res.Body).Decode(&response)
	if err != nil {
		return nil, err
	}

	return &response, nil
}

func printPlaceDescription(descr *InterestingPlaceDescriptionResponse) {
	fmt.Printf("%s\n", descr.Title)
    fmt.Printf("\tDescription: %s\n", descr.Description)
    fmt.Printf("\tLocation: %s\n", descr.Location)
    fmt.Printf("\tPhone: %s\n", descr.Phone)
}

type LocationData struct {
	weather           *WeatherResponse
	interestingPlaces []*InterestingPlaceDescriptionResponse
}

func (data *LocationData) collectInterestingPlacesDescriptions(places *InterestingPlacesResponse) {
    var descrWG sync.WaitGroup
    descrChan := make(chan *InterestingPlaceDescriptionResponse)
    for _, place := range places.Results {
        descrWG.Add(1)
        go func() {
            defer descrWG.Done()
            response, err := interestingPlaceDescriptionRequest(place.ID)
            if (err != nil) {
                fmt.Println("Failed to receive interesting place description: ", err)
            }
            descrChan <- response
        }()
    }

    go func() {
        descrWG.Wait()
        close(descrChan)
    }()

    data.interestingPlaces = make([]*InterestingPlaceDescriptionResponse, 0)
    for descr := range descrChan {
        data.interestingPlaces = append(data.interestingPlaces, descr)
    }
}

func (data *LocationData) collectLocationData(lat, lng float64) {
	weatherChan := make(chan *WeatherResponse, 1)
	go func() {
        response, err := weatherRequest(lat, lng)
        if err != nil {
            fmt.Println("Failed to receive weather: ", err)
        }
		weatherChan <- response
	}()

	interestingPlacesChan := make(chan *InterestingPlacesResponse, 1)
	go func() {
        response, err := interestingPlacesRequest(lat, lng)
        if (err != nil) {
            fmt.Println("Failed to receive interesting places list: ", err)
        }
		interestingPlacesChan <- response
	}()

	interestingPlaces := <-interestingPlacesChan

    if interestingPlaces != nil {
        data.collectInterestingPlacesDescriptions(interestingPlaces)
    }

	data.weather = <-weatherChan
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

    spinner := spinner.New(spinner.CharSets[12], 150 * time.Millisecond)
    spinner.Start()

	geocodingResponse, err := geocodingRequest(placeName)
    spinner.Stop()
    if err != nil {
        log.Fatal("Failed to receive geocoding data: ", err)
    }

	placeNum := choosePlace(geocodingResponse)

	lat := geocodingResponse.Hits[placeNum].Point.Lat
	lng := geocodingResponse.Hits[placeNum].Point.Lng


    spinner.Start()
    var data LocationData
    data.collectLocationData(lat, lng)
    spinner.Stop()

    if data.weather != nil {
        weatherPrettyPrint(data.weather)
    }

    fmt.Println("Interesting places:")
	for _, descr := range data.interestingPlaces {
		printPlaceDescription(descr)
	}
}
