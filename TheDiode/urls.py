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
    path('rgbframe/explore<int:frame_id>/', rgbframe_explore, name='rgbframe_explore'),
    path('rgbframe/explore<int:frame_id>/sequencepattern<int:seqpatt_id>', rgbframe_explore_sequencepattern, name='rgbframe_explore_sequencepattern'),
    path('rgbframeEditSeqPatt<int:frame_id>/<int:seqpatt_id>', rgbframe_edit_sequencepattern, name='rgbframeEditSeqPatt'),
    path('rgbframeDelSeqPatt<int:frame_id>/<int:seqpatt_id>', rgbframe_del_sequencepattern, name='rgbframeDelSeqPatt'),
    path('rgbframeAddSeqPatt<int:frame_id>/', rgbframe_add_sequencepattern, name='rgbframeAddSeqPatt'),
    path('rgbframeCreateSeqPatt<int:frame_id>/', rgbframe_create_sequencepattern, name='rgbframeCreateSeqPatt'),
    path('rgbframeAddSequence<int:seqpatt_id>/', rgbframe_add_sequence, name='rgbframeAddSequence'),
    path('rgbframeCopySequence<int:seqpatt_id>/', rgbframe_copy_sequence, name='rgbframeCopySequence'),
    path('rgbframeEditSequence<int:seqpatt_id>/<int:seq_id>', rgbframe_edit_sequence, name='rgbframeEditSequence'),
    path('rgbframeDelSequence<int:seqpatt_id>/<int:seq_id>', rgbframe_del_sequence, name='rgbframeDelSequence'),
    path('rgbframeAddPattern<int:seqpatt_id>/<int:seq_id>', rgbframe_add_pattern, name='rgbframeAddPattern'),
    path('rgbframeCopyPattern<int:seqpatt_id>/<int:seq_id>/<int:patt_id>', rgbframe_copy_pattern, name='rgbframeCopyPattern'),
    path('rgbframeEditPattern<int:seqpatt_id>/<int:seq_id>/<int:patt_id>', rgbframe_edit_pattern, name='rgbframeEditPattern'),
    path('rgbframeDelPattern<int:seqpatt_id>/<int:seq_id>/<int:patt_id>', rgbframe_del_pattern, name='rgbframeDelPattern'),
    path('cyanConverter', cyan_converter, name='cyan_converter'),
    ] + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
