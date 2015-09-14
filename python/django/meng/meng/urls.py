from django.conf.urls import include, url
from django.contrib import admin
import hello
import books.views
import contact.views

urlpatterns = [
    # Examples:
    # url(r'^$', 'meng.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    url(r'^admin/', include(admin.site.urls)),
    url(r'^hello/(?P<name>\w+)$', hello.greeting, { 'message' : 'hello'}),
    url(r'^bye/(?P<name>\w+)$', hello.greeting, { 'message' : 'bye'}),
    url(r'^header/$', hello.display_head),
    url(r'^search/$', books.views.search),
    url(r'^time/$', hello.time),
    url(r'^header/$', hello.display_head),
    url(r'^contact/$', contact.views.contact),
    url(r'^contact/thanks/$', contact.views.contact_thanks),
]
