from django.core.mail import send_mail
from django.shortcuts import render
from django.http import HttpResponseRedirect, HttpResponse
from django.template.context_processors import csrf
from forms import ContactForm


def contact(request):
    if request.method == 'POST':
        form = ContactForm(request.POST)
        if form.is_valid():
            cd = form.cleaned_data
            send_mail(
                cd['subject'],
                cd['message'],
                cd.get('email', '287819708@qq.com'),
                ['fancmong@mail.ustc.edu.cn'],
                fail_silently = False
            )
            return HttpResponseRedirect('/contact/thanks')
    else:
        form = ContactForm(initial = {'subject' : 'I love your site!'})
    return render(request, 'contact_form_custom.html', { 'form' : form })

def contact_thanks(request):
    return HttpResponse('<html><p>Thanks for contacting us!</p></html>')
