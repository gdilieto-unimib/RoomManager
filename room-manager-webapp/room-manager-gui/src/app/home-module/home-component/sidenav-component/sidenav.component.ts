import { Component } from "@angular/core";
import { ConfigurationService } from "../services/configuration.service";

@Component({
    selector: 'app-sidenav',
    templateUrl: './sidenav.component.html',
    styleUrls: ['./sidenav.component.css']
})
export class SidenavComponent{

    constructor(public configurationService: ConfigurationService) {}

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
        this.configurationService.setEcoMode(sleepMode).subscribe(
            (response) => {
                
            }
        )
    }

}