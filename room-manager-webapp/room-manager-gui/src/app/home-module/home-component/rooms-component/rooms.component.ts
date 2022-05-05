import { Component, Input } from "@angular/core";
import { roomsMock } from "./mocks/rooms.mock";
import { defaultRoom, Room } from "./room-component/models/room.model";

@Component({
    selector: 'app-rooms',
    templateUrl: './rooms.component.html',
    styleUrls: ['./rooms.component.css']
})
export class RoomsComponent{
    @Input() rooms: Room[] = roomsMock

    constructor() {
    }
}