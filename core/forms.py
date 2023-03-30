from django.forms import ModelForm
from componente.models import *
from projetos.models import *

class CriarCategoriaForm(ModelForm):
    def __init__(self,*args,**kwargs):
        super(CriarCategoriaForm, self).__init__(*args, **kwargs)
        self.fields['nome'].widget.attrs.update({'class': 'criacao'})

    class Meta:
        model = Categoria
        fields = ['nome']

class CriarTipoForm(ModelForm):
    def __init__(self,id_categoria,*args,**kwargs):
        super (CriarTipoForm,self ).__init__(*args,**kwargs) # populates the post
        self.fields['categoria'].queryset = Categoria.objects.filter(id=id_categoria)
        self.fields['nome'].widget.attrs.update({'class': 'criacao'})
        self.fields['categoria'].widget.attrs.update({'class': 'criacao'})

    class Meta:
        model = TipoDeComponente
        fields = ['nome',
                  'categoria']

class CriarComponenteForm(ModelForm):
    def __init__(self,id_tipo,*args,**kwargs):
        super (CriarComponenteForm,self ).__init__(*args,**kwargs) # populates the post
        self.fields['tipo'].queryset = TipoDeComponente.objects.filter(id=id_tipo)
        self.fields['nome'].widget.attrs.update({'class': 'criacao'})
        self.fields['tipo'].widget.attrs.update({'class': 'criacao'})
        self.fields['quantidade'].widget.attrs.update({'class': 'criacao'})

    class Meta:
        model = Componente
        fields = ['nome',
                  'tipo',
                  'quantidade']

class ComponenteForm(ModelForm):
    class Meta:
        model = Componente
        fields = ['quantidade']

class CriarPainelForm(ModelForm):
    def __init__(self,criar_painel_ids,*args,**kwargs):
        super (CriarPainelForm,self ).__init__(*args,**kwargs) # populates the post
        i = 1
        for id_componente in criar_painel_ids.split(','):
            self.fields['slot_'+str(i)].queryset = Componente.objects.filter(id=int(id_componente))
            self.fields['slot_'+str(i)].widget.attrs.update({'class': 'criacao'})
            i += 1
        self.fields['nome'].widget.attrs.update({'class': 'criacao'})
        self.fields['base'].widget.attrs.update({'class': 'criacao'})
    class Meta:
        model = PainelArmazenamentoModular
        fields = ['nome',
                  'base',
                  'slot_1',
                  'slot_2',
                  'slot_3',
                  'slot_4',
                  'slot_5',
                  'slot_6',
                  'slot_7',
                  'slot_8',
                  'slot_9',]