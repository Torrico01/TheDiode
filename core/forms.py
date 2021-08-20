from django.forms import ModelForm
from componente.models import Componente, Categoria, TipoDeComponente

class ComponenteForm(ModelForm):
    class Meta:
        model = Componente
        fields = ['quantidade']