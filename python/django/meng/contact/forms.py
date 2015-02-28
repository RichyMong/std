from django import forms


class ContactForm(forms.Form):
    email = forms.EmailField(required = False)
    subject = forms.CharField()
    message = forms.CharField(widget = forms.Textarea)

    def clean_message(self):
        message = self.cleaned_data['message']
        num = len(message.split())
        if num < 4:
            raise forms.ValidationError("Not enough words!")
        return message
