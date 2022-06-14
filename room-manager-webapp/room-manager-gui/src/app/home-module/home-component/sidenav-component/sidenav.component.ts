import { Component, OnInit } from "@angular/core";
import { MatDialog } from "@angular/material/dialog";
import { EditScheduleDialog } from "./services/edit-schedule-dialog/edit-schedule.dialog";
import { ConfigurationService } from "../services/configuration.service";
import { Configuration, defaultConfiguration } from "../rooms-component/models/configuration.model";

@Component({
    selector: 'app-sidenav',
    templateUrl: './sidenav.component.html',
    styleUrls: ['./sidenav.component.css']
})
export class SidenavComponent {

    constructor(
        public configurationService: ConfigurationService,
        public dialog: MatDialog
    ) {}

    onSingleModeToggle(singleMode: boolean) {
        this.configurationService.setSingleMode(singleMode).subscribe(
            (response) => {
                
            }
        )
    }

    onEcoModeToggle(ecoMode: boolean) {
        this.configurationService.setEcoMode(ecoMode).subscribe(
            (response) => {
                
            }
        )
    }

    onSleepModeToggle(sleepMode: boolean) {
        this.configurationService.setSleepMode(sleepMode).subscribe(
            (response) => {
                
            }
        )
    }

    editSchedule() {
        const dialogRef = this.dialog.open(EditScheduleDialog, {
            data: {
                sleepSchedule: this.configurationService.configuration.sleepSchedule
            }
        });

        dialogRef.afterClosed().subscribe(editedSchedule => {
            if(editedSchedule) {
                this.configurationService.setSleepSchedule(editedSchedule).subscribe(
                    sleepSchedule => {

                    }
                )
            }
        });
    }

}