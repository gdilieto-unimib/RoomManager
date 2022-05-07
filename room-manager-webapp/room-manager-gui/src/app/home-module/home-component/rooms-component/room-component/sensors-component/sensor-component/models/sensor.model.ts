export enum SensorType {
    Temperature = 'Temperature',
    Light = 'Light',
    Wifi = 'Wifi'
}

export interface Sensor{
    type?: SensorType,
    name?: string,
    measure?: string
}

export const defaultSensor: Sensor = {
    name: "", 
    measure:"N.A."
}