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

from core.views import *

urlpatterns = [
    path('', home, name='home'),
    path('admin/', admin.site.urls),
    path("favicon.ico", RedirectView.as_view(url=staticfiles_storage.url("favicon.ico"))),
    path('componentes/', categorias, name='categorias'),
    path('componentes/search/', search, name='search'),
    path('componentes/criarcategoria/', criarcategoria, name='criarcategoria'),
    path('componentes/categoria<int:id>/', tiposdecomponentes, name='tiposdecomponentes'),
    path('componentes/categoria<int:id>/criartipo/', criartipo, name='criartipo'),
    path('componentes/categoria<int:id_categoria>/tipo<int:id>/', componentes, name='componentes'),
    path('componentes/categoria<int:id_categoria>/tipo<int:id>/criarcomponente/', criarcomponente, name='criarcomponente'),
    path('componentes/categoria<int:id_categoria>/tipo<int:id_tipo>/componente<int:id>', modificar, name='modificar'),
    path('painel/', paineldearmazenamentomodular, name='paineldearmazenamentomodular'),
    path('painel/criarpainel', criarpaineldearmazenamentomodular, name='criarpaineldearmazenamentomodular'),
    path('rgbframe/', rgbframe, name='rgbframe'),
    path('rgbframe/explore<int:id>/', rgbframe_explore, name='rgbframe_explore'),
    path('rgbframeAddSequence<int:id>/', rgbframe_add_sequence, name='rgbframeAddSequence'),
    path('rgbframeCopySequence<int:id>/', rgbframe_copy_sequence, name='rgbframeCopySequence'),
    path('rgbframeEditSequence<int:id>/<int:seq_id>', rgbframe_edit_sequence, name='rgbframeEditSequence'),
    path('rgbframeDelSequence<int:id>/<int:seq_id>', rgbframe_del_sequence, name='rgbframeDelSequence'),
    path('rgbframeAddPattern<int:id>/<int:seq_id>', rgbframe_add_pattern, name='rgbframeAddPattern'),
    path('rgbframeCopyPattern<int:id>/<int:seq_id>/<int:patt_id>', rgbframe_copy_pattern, name='rgbframeCopyPattern'),
    path('rgbframeEditPattern<int:id>/<int:seq_id>/<int:patt_id>', rgbframe_edit_pattern, name='rgbframeEditPattern'),
    path('rgbframeDelPattern<int:id>/<int:seq_id>/<int:patt_id>', rgbframe_del_pattern, name='rgbframeDelPattern'),
    ] + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
