import { Room } from "../models/room.model"

export const roomMock1 : Room = {
    id: 1,
    name: "Room1",
    ipv4: "111.11.11.11",
    connected: true,
    sensors: []
}
export const roomMock2 : Room = {
    id: 2,
    name: "Room2",
    ipv4: "222.22.22.22",
    connected: false,
    sensors: []
}