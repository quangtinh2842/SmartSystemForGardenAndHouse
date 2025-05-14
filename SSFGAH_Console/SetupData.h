// id 1xx: Living room
// id 2xx: Bedroom
// id 3xx: Kitchen
// id 4xx: Garden

const char SETUP_DATA[] PROGMEM = R"=====(
  {
    "nPieces": 32,
    "pieceArray": [
        {
            "id": "100",
            "name": "252 Recourse",
            "sign": "252",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Living Room",
            "eventCode": "252"
        },
        {
            "id": "101",
            "name": "Thief Detection",
            "sign": "Thi",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Living Room",
            "eventCode": "thief"
        },
        {
            "id": "102",
            "name": "Light Detection",
            "sign": "LD",
            "type": "SENSOR_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Living Room",
            "eventCode": ""
        },
        {
            "id": "103",
            "name": "Porch Light",
            "sign": "PL",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Living Room",
            "eventCode": ""
        },
        {
            "id": "104",
            "name": "Porch Light A Mode",
            "sign": "PLAM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Living Room",
            "eventCode": ""
        },
        {
            "id": "105",
            "name": "Light 1",
            "sign": "L1",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Living Room",
            "eventCode": ""
        },
        {
            "id": "106",
            "name": "Light 2",
            "sign": "L2",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Living Room",
            "eventCode": ""
        },
        {
            "id": "107",
            "name": "Clap Switch Mode",
            "sign": "CSM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Living Room",
            "eventCode": ""
        },
        {
            "id": "200",
            "name": "Temperature",
            "sign": "Te",
            "type": "SENSOR_TYPE",
            "valueType": "INT_TYPE",
            "value": "0",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "201",
            "name": "Humidity",
            "sign": "Hu",
            "type": "SENSOR_TYPE",
            "valueType": "INT_TYPE",
            "value": "0",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "202",
            "name": "252 Recourse",
            "sign": "252",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Bedroom",
            "eventCode": "252"
        },
        {
            "id": "203",
            "name": "Light",
            "sign": "L",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "204",
            "name": "Clap Switch Mode",
            "sign": "CSM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "205",
            "name": "Fan",
            "sign": "Fa",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "206",
            "name": "Fan A Mode",
            "sign": "FaAM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "207",
            "name": "Fan Te Thresh",
            "sign": "FaTT",
            "type": "CONTROL_TYPE",
            "valueType": "INT_TYPE",
            "value": "23",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "208",
            "name": "Heater",
            "sign": "He",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "209",
            "name": "Heater A Mode",
            "sign": "HeAM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "210",
            "name": "Heater Te Thresh",
            "sign": "HeTeT",
            "type": "CONTROL_TYPE",
            "valueType": "INT_TYPE",
            "value": "18",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "211",
            "name": "Humidifier",
            "sign": "H",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "212",
            "name": "Humidifier A Mode",
            "sign": "HAM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "213",
            "name": "H Hu Thresh",
            "sign": "HHuT",
            "type": "CONTROL_TYPE",
            "valueType": "INT_TYPE",
            "value": "40",
            "fromModule": "Bedroom",
            "eventCode": ""
        },
        {
            "id": "300",
            "name": "Smoke",
            "sign": "Sm",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Kitchen",
            "eventCode": "smoke"
        },
        {
            "id": "301",
            "name": "Gas",
            "sign": "Gas",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Kitchen",
            "eventCode": "gas"
        },
        {
            "id": "302",
            "name": "252 Recourse",
            "sign": "252",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Kitchen",
            "eventCode": "252"
        },
        {
            "id": "303",
            "name": "Light 1",
            "sign": "L1",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Kitchen",
            "eventCode": ""
        },
        {
            "id": "304",
            "name": "Light 2",
            "sign": "L2",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Kitchen",
            "eventCode": ""
        },
        {
            "id": "305",
            "name": "Clap Switch Mode",
            "sign": "CSM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Kitchen",
            "eventCode": ""
        },
        {
            "id": "400",
            "name": "Thief Detection",
            "sign": "Thi",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Garden",
            "eventCode": "thief"
        },
        {
            "id": "401",
            "name": "Rain Detection",
            "sign": "Rain",
            "type": "EVENT_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Garden",
            "eventCode": "rain"
        },
        {
            "id": "402",
            "name": "Light",
            "sign": "L",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "0",
            "fromModule": "Garden",
            "eventCode": ""
        },
        {
            "id": "403",
            "name": "Light A Mode",
            "sign": "LAM",
            "type": "CONTROL_TYPE",
            "valueType": "BOOL_TYPE",
            "value": "1",
            "fromModule": "Garden",
            "eventCode": ""
        }
    ]
  }
)=====";