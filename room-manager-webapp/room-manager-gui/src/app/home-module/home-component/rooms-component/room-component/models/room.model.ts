import { Sensor } from "../sensors-component/sensor-component/models/sensor.model";

export interface Room{
    id?: number,
    name: string,
    ipv4: string,
    connected?: boolean,
    ssid?: string,
    password?: string,
    sensors: Sensor[]
}

export const defaultRoom : Room = {
    name: "",
    ipv4: "",
    sensors: []
}