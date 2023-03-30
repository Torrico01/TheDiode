"""TheDiode URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/3.1/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path, include
from django.conf import settings
from django.conf.urls.static import static
from django.views.generic.base import RedirectView
from django.contrib.staticfiles.storage import staticfiles_storage

from core.views import (
    home,
    search,
    tipos_de_componentes,
    componentes,
    modificar,
    criarcategoria,
    criartipo,
    criarcomponente,
    paineldearmazenamentomodular,
    criarpaineldearmazenamentomodular
)

urlpatterns = [
    path('', home, name='home'),
    path('admin/', admin.site.urls),
    path('search/', search, name='search'),
    path('criar/', criarcategoria, name='criarcategoria'),
    path('<int:id>/', tipos_de_componentes, name='tipos_de_componentes'),
    path('<int:id>/criar/', criartipo, name='criartipo'),
    path('<int:id_categoria>/<int:id>/', componentes, name='componentes'),
    path('<int:id_categoria>/<int:id>/criar/', criarcomponente, name='criarcomponente'),
    path('<int:id_categoria>/<int:id_tipo>/<int:id>', modificar, name='modificar'),
    path('painel/',paineldearmazenamentomodular, name='paineldearmazenamentomodular'),
    path('painel/criar',criarpaineldearmazenamentomodular, name='criarpaineldearmazenamentomodular'),
    path("favicon.ico", RedirectView.as_view(url=staticfiles_storage.url("favicon.ico"))),
    ] + static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)
