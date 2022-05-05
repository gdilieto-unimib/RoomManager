import { Component, Input } from "@angular/core";
import { defaultRoom, Room } from "./models/room.model";

@Component({
    selector: 'app-room',
    templateUrl: './room.component.html',
    styleUrls: ['./room.component.css']
})
export class RoomComponent{
    @Input() room: Room = defaultRoom
}