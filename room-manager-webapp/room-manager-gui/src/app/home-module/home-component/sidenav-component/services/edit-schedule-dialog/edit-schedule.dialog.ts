import { Component, Inject } from "@angular/core";
import { MatDialogRef, MAT_DIALOG_DATA } from "@angular/material/dialog";
import { RoomsService } from "../../../rooms-component/services/rooms.service";

export interface EditScheduleDialogData {
  sleepSchedule: String;
}

@Component({
  selector: 'edit-schedule',
  templateUrl: 'edit-schedule.dialog.html',
  styleUrls: ['edit-schedule.dialog.css'],
})
export class EditScheduleDialog {
  hours = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23]
  sleepSchedule: String
  
  constructor(
    public dialogRef: MatDialogRef<EditScheduleDialog>,
    @Inject(MAT_DIALOG_DATA) public data: EditScheduleDialogData,
    private roomsService: RoomsService
  ) {
    this.sleepSchedule = data.sleepSchedule
    console.log(this.sleepSchedule)
  }

  onConfirmClick(): void {
    this.dialogRef.close(this.sleepSchedule);
  }

  onCancelClick(): void {
    this.dialogRef.close(false);
  }

  toggleHour(i: number) {
    this.sleepSchedule =this.setCharAt(this.sleepSchedule, i, this.sleepSchedule[i]=="1"?"0":"1")
  }

  setCharAt(str: String, index: number, chr: String) {
      if(index > str.length-1) return str;
      return str.substring(0,index) + chr + str.substring(index+1);
  }
  
}