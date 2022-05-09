import { AbstractControl, FormBuilder, FormGroup } from "@angular/forms";

export abstract class Form<T> {
    form : FormGroup = new FormGroup({})

    constructor(public formBuilder: FormBuilder){
    }

    abstract createForm(object: T): FormGroup;

    abstract updateForm(object: T): FormGroup;

    get(field: string): AbstractControl | null {
        return this.form.get(field);
    } 
    
    getValue(field: string): any {
        return this.form.get(field)?.value;
    } 

    getFormValue(): T {
        return this.form.getRawValue();
    }

    isValid(): boolean {
        return this.form.valid;
    }
}