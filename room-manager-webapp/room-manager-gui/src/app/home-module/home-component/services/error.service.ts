import { Injectable } from '@angular/core';
import { MatSnackBar, MatSnackBarRef } from '@angular/material/snack-bar';
import { Error } from '../models/error.model';

@Injectable({
    providedIn: 'root',
})
export class ErrorService {

    private snackBarRef: any;

    constructor(private _snackBar: MatSnackBar) { }

    showError(error: Error) {
        return this._snackBar.open(
            "Error code: "+error.errorCode+"\n"+"Error message: "+error.errorMessage,
            undefined,
            { duration: 3000, panelClass: "red-snackbar" });
    }

    showErrors(errors: Error[]) {
        if (errors.length>0) {   
            let error: Error|undefined = errors.pop()
            if(error) {
                this.showError(error).afterDismissed().subscribe((
                    () => {
                        this.showErrors(errors)
                    }
                ))
            }
        }
    }
}