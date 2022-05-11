import { Component, EventEmitter, Input, OnInit, Output } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { RoomsService } from "../services/rooms.service";
import { DeleteRoomDialog } from "./dialogs/delete-room-dialog/delete-room.dialog";
import { EditRoomDialog } from "./dialogs/edit-room-dialog/edit-room.dialog";
import { defaultRoom, Room } from "./models/room.model";

@Component({
    selector: 'app-room',
    templateUrl: './room.component.html',
    styleUrls: ['./room.component.scss']
})
export class RoomComponent implements OnInit {
    @Input() room: Room = defaultRoom
    @Output() editedRoom: EventEmitter<Room> = new EventEmitter()
    @Output() deletedRoom: EventEmitter<Room> = new EventEmitter()
    
    isConnecting = false;
    isUpdatePolling = false

    constructor(
        public dialog: MatDialog,
        public roomsService: RoomsService
    ) {}

    ngOnInit() {
        this.isConnectedRoomPolling();
    }

    isConnectedUpdate() : void {
        this.getIsRoomConnected(this.room.ipv4).subscribe(
            (isConnected) => {
                this.room.connected = isConnected
                if (this.room.connected && this.room.monitoring) {
                    if (this.isUpdatePolling == false) {
                        this.updateRoomPolling()
                    } 
                    this.isUpdatePolling = true
                } else {

                    if (!this.room.connected) {
                        this.room.monitoring = false
                    }
                    this.isUpdatePolling = false
                }
            },
            (err) => {
                this.room.connected = false
                this.room.monitoring = false
            }
        )
    }

    isConnectedRoomPolling(): void {
        this.isConnectedUpdate();
        setTimeout(() => {
            this.isConnectedRoomPolling();
        },5000);
    }

    updateRoomPolling(): void {
        if(this.room.connected) {
            this.getRoomSensors(this.room.id?this.room.id:0).subscribe(
                (sensors) => {
                    this.room.sensors = sensors
                }
            )
            this.getRoomAlarms(this.room.id?this.room.id:0).subscribe(
                (alarms) => {
                    this.room.alarms = alarms.map(
                        alarm => {
                            let alarmDate = new Date(alarm.datetime)
                            let millis = alarmDate.getMilliseconds()
                            alarmDate.setMilliseconds(millis + 7200000)
                            alarm.datetime = alarmDate
                            return alarm
                        }
                    )
                }
            )
            setTimeout(() => {
                this.updateRoomPolling();
            },5000);
        }
    }

    toggleMonitoring(): void {
        this.isConnecting = true
        if(this.room.monitoring) {
            this.postStopRoomMonitoring(this.room.ipv4).subscribe(
                stopped => {
                    if (stopped) {
                        this.room.monitoring = false
                    }
                    this.isConnecting = false
                }
            )
        } else {
            this.postStartRoomMonitoring(this.room.ipv4).subscribe(
                started => {
                    if (started) {
                        this.room.monitoring = true
                    }
                    this.isConnecting = false
                }
            )
        }
    }

    editRoom(): void {
        const dialogRef = this.dialog.open(EditRoomDialog, {
            data: {
                room: this.room
            }
        });

        dialogRef.afterClosed().subscribe(editedRoom => {
            if(editedRoom) {
                this.editedRoom.emit(editedRoom)
            }
        });
    } 
    
    deleteRoom() {
        const dialogRef = this.dialog.open(DeleteRoomDialog, {
            data: {
                room: this.room
            }
        });

        dialogRef.afterClosed().subscribe(deletedRoom => {
            if(deletedRoom) {
                this.deletedRoom.emit(this.room)
            }
        });
    }

    getAlarmClass(alarmCode: string): string[] {
        switch (Number(alarmCode)) {
            case 0: {
                return ["red-alarm"]
            }
            case 1: {
                return ["blue-alarm"]
            }
            default: {
                return []
            }
        } 
    }

    getRoom(roomId: number) {
        return this.roomsService.getRoom(roomId)
    }

    getRoomSensors(roomId: number) {
        return this.roomsService.getRoomSensors(roomId)
    }

    getRoomAlarms(roomId: number) {
        return this.roomsService.getRoomAlarms(roomId)
    }

    getIsRoomConnected(roomIp: string) {
        return this.roomsService.getIsRoomConnected(roomIp)
    }

    postStartRoomMonitoring(roomIp: string) {
        return this.roomsService.postStartRoomMonitoring(roomIp)
    }

    postStopRoomMonitoring(roomIp: string) {
        return this.roomsService.postStopRoomMonitoring(roomIp)
    }
}