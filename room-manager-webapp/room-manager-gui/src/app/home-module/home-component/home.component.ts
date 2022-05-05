import { Component } from '@angular/core';

@Component({
    selector: 'app-home',
    templateUrl: './home.component.html',
    styleUrls: ['./home.component.css']
})
export class HomeComponent {
    onToggle(toggle: boolean){
        //drawer.toggle(event)
        console.log("CAO"+toggle)
    }
}