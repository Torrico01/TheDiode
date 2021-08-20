from django.forms import ModelForm
from componente.models import Componente, Categoria, TipoDeComponente

class CriarCategoriaForm(ModelForm):
    class Meta:
        model = Categoria
        fields = ['nome']

class CriarTipoForm(ModelForm):
    def __init__(self,id_categoria,*args,**kwargs):
        super (CriarTipoForm,self ).__init__(*args,**kwargs) # populates the post
        self.fields['categoria'].queryset = Categoria.objects.filter(id=id_categoria)

    class Meta:
        model = TipoDeComponente
        fields = ['nome',
                  'categoria']

class CriarComponenteForm(ModelForm):
    def __init__(self,id_tipo,*args,**kwargs):
        super (CriarComponenteForm,self ).__init__(*args,**kwargs) # populates the post
        self.fields['tipo'].queryset = TipoDeComponente.objects.filter(id=id_tipo)

    class Meta:
        model = Componente
        fields = ['nome',
                  'tipo',
                  'quantidade']

class ComponenteForm(ModelForm):
    class Meta:
        model = Componente
        fields = ['quantidade']