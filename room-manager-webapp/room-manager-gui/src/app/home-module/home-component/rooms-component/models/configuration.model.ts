export interface Configuration {
    singleMode: boolean,
    ecoMode: boolean,
    sleepMode: boolean,
    sleepSchedule: String,
}

export const defaultConfiguration : Configuration = {
    singleMode: false,
    ecoMode: false,
    sleepMode: false,
    sleepSchedule: "00000000000000000000000000",
}