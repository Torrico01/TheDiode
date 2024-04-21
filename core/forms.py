from django.forms import ModelForm
from django.forms.widgets import TextInput
from django import forms
from components.models import *
from projects.models import *
from storage.models import *

class CriarCategoriaForm(ModelForm):
    def __init__(self,*args,**kwargs): # populates the post
        super(CriarCategoriaForm, self).__init__(*args, **kwargs)
        self.fields['name'].widget.attrs.update({'class': 'criacao'})

    class Meta:
        model = Category
        fields = ['name']

class CriarTipoForm(ModelForm):
    def __init__(self,id_categoria,*args,**kwargs):
        super (CriarTipoForm,self ).__init__(*args,**kwargs) # populates the post
        self.fields['categoria'].queryset = Category.objects.filter(id=id_categoria)
        self.fields['name'].widget.attrs.update({'class': 'criacao'})
        self.fields['categoria'].widget.attrs.update({'class': 'criacao'})

    class Meta:
        model = ComponentType
        fields = ['name',
                  'categoria']

class CriarComponenteForm(ModelForm):
    def __init__(self,id_tipo,*args,**kwargs):
        super (CriarComponenteForm,self ).__init__(*args,**kwargs) # populates the post
        self.fields['tipo'].queryset = ComponentType.objects.filter(id=id_tipo)
        self.fields['name'].widget.attrs.update({'class': 'criacao'})
        self.fields['tipo'].widget.attrs.update({'class': 'criacao'})
        self.fields['quantidade'].widget.attrs.update({'class': 'criacao'})

    class Meta:
        model = Component
        fields = ['name',
                  'tipo',
                  'quantidade']

class ComponenteForm(ModelForm):
    class Meta:
        model = Component
        fields = ['quantidade']

class CriarPainelForm(ModelForm):
    def __init__(self,criar_painel_ids,*args,**kwargs):
        super (CriarPainelForm,self ).__init__(*args,**kwargs) # populates the post
        i = 1
        for id_componente in criar_painel_ids.split(','):
            self.fields['slot_'+str(i)].queryset = Component.objects.filter(id=int(id_componente))
            self.fields['slot_'+str(i)].widget.attrs.update({'class': 'criacao'})
            i += 1
        self.fields['name'].widget.attrs.update({'class': 'criacao'})
        self.fields['base'].widget.attrs.update({'class': 'criacao'})
    class Meta:
        model = ModularStoragePanel3x3
        fields = ['name',
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
        
class RGBFrameAddSequenceForm(forms.Form): # Used for editing, copying and adding
    L1 = forms.IntegerField(label="Initial LED")
    L2 = forms.IntegerField(label="Final LED")
    T = forms.IntegerField(label="Time")
    Color1 = forms.CharField(label='Initial Color', max_length=7,
        widget=forms.TextInput(attrs={'type': 'color'}))
    Color2 = forms.CharField(label='Final Color', max_length=7,
        widget=forms.TextInput(attrs={'type': 'color'}))
    
class RGBFrameEditSequenceForm(forms.Form): # Used for mass editing
    T = forms.IntegerField(label="Time")
    Color1 = forms.CharField(label='Initial Color', max_length=7,
        widget=forms.TextInput(attrs={'type': 'color'}))
    Color2 = forms.CharField(label='Final Color', max_length=7,
        widget=forms.TextInput(attrs={'type': 'color'}))

class RGBFrameAddSeqPattForm(forms.Form): # Used to edit and add new sequence pattern to the rgb strip sequence    
    all_framesequences = RGBFrameSequences.objects.all()
    for obj in all_framesequences:
        obj.refresh_from_db()
    names_array = list(all_framesequences.values_list('name', flat=True))
    choices_array = []
    for name in names_array:
        choices_array.append((name, name))
    name = forms.CharField(label='Name', widget=forms.Select(choices=choices_array))
    times = forms.IntegerField(label="Times", initial=1)
    speed = forms.IntegerField(label="Speed", initial=1)

class RGBFrameCreateSeqPattForm(forms.Form): # Used to create a new sequence pattern
    name = forms.CharField(label="Name", max_length=100)