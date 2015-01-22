from django.conf.urls import include, url
from django.contrib import admin
import hello
import books.views

urlpatterns = [
    # Examples:
    # url(r'^$', 'meng.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    url(r'^admin/', include(admin.site.urls)),
    url(r'^time/$', hello.time),
    url(r'^header/$', hello.display_head),
    url(r'^search-form/$', books.views.search_form),
    url(r'^search/$', books.views.search),
]
