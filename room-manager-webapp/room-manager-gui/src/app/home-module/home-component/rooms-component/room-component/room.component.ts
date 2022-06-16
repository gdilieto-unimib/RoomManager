import { Component, EventEmitter, Input, OnInit, Output } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { ErrorService } from "../../services/error.service";
import { RoomsService } from "../services/rooms.service";
import { DeleteRoomDialog } from "./dialogs/delete-room-dialog/delete-room.dialog";
import { EditRoomDialog } from "./dialogs/edit-room-dialog/edit-room.dialog";
import { defaultRoom, Room } from "./models/room.model";

@Component({
    selector: 'app-room',
    templateUrl: './room.component.html',
    styleUrls: ['./room.component.scss']
})
export class RoomComponent {
    @Input() room: Room = defaultRoom
    @Output() editedRoom: EventEmitter<Room> = new EventEmitter()
    @Output() deletedRoom: EventEmitter<Room> = new EventEmitter()
    
    isConnecting = false;

    constructor(
        public dialog: MatDialog,
        public roomsService: RoomsService,
        public errorService: ErrorService
    ) {}

    toggleMonitoring(): void {
        this.isConnecting = true
        this.roomsService.updatingControl = true
        if(this.room.monitoring) {
            this.setMonitoringStop(this.room.id?this.room.id:-1).subscribe(
                stopped => {
                    this.roomsService.updatingControl = false
                    this.room.monitoring = stopped.monitoringActivated
                    this.isConnecting = false
                },
                err => {
                    this.roomsService.updatingControl = false
                    this.isConnecting = false
                    this.errorService.showErrors(err.error)
                }
            )
        } else {
            this.setMonitoringStart(this.room.id?this.room.id:-1).subscribe(
                started => {
                    this.roomsService.updatingControl = false
                    this.room.monitoring = started.monitoringActivated
                    this.isConnecting = false
                },
                err => {
                    this.roomsService.updatingControl = false
                    this.isConnecting = false
                    this.errorService.showErrors(err.error)
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

    getIsRoomConnected(roomId: number) {
        return this.roomsService.getIsRoomConnected(roomId)
    }

    setMonitoringStart(roomId: number) {
        return this.roomsService.setMonitoringStart(roomId)
    }

    setMonitoringStop(roomId: number) {
        return this.roomsService.setMonitoringStop(roomId)
    }
}