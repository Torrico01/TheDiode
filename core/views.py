from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.models import User
from django.urls import reverse

from .forms import ComponenteForm
from componente.models import TipoDeComponente, Categoria, Componente

def conta_componentes():
    categorias = Categoria.objects.all()
    count = 0
    count_total = 0
    for categoria in categorias:
        tipodecomponentes_filtrados = categoria.tipodecomponente_set.all()
        for tipodecomponente in tipodecomponentes_filtrados:
            componentes_filtrados = tipodecomponente.componente_set.all()
            for componente_filtrado in componentes_filtrados:
                count += componente_filtrado.quantidade
            tipodecomponente.quantidade = count
            tipodecomponente.save()
            count_total += count
            count = 0
        categoria.quantidade = count_total
        categoria.save()
        count_total = 0

def home(request):
    conta_componentes()
    componentes = Componente.objects.all()
    tipodecomponentes = TipoDeComponente.objects.all()
    categorias = Categoria.objects.all()
    
    context = {'componentes': componentes,
                'categorias':categorias,
                'tipodecomponentes':tipodecomponentes}
    return render(request, 'core/menu.html', context)

def search(request):
    conta_componentes()
    q=request.GET['q']
    componentes = Componente.objects.filter(nome__icontains=q).order_by('-id')
    tipodecomponentes = TipoDeComponente.objects.all()
    categorias = Categoria.objects.all()
    context = {'componentes':componentes, 
                'categorias':categorias, 
                'tipodecomponentes':tipodecomponentes}
    return render(request, 'core/search.html', context)

def tipos_de_componentes(request, id):
    conta_componentes()
    componentes = Componente.objects.all()
    tipodecomponente = TipoDeComponente.objects.filter(categoria=id)
    categorias = Categoria.objects.all()

    context = {'componentes': componentes, 
                'categorias':categorias, 
                'tipodecomponentes':tipodecomponente,
                'categoria_especifica':Categoria.objects.get(id=id),
                'id_antigo':id}
    return render(request, 'core/tipos_de_componentes.html', context)

def componentes(request, id_antigo, id):
    conta_componentes()
    componentes = Componente.objects.filter(tipo=id)
    tipodecomponente = TipoDeComponente.objects.filter(categoria=id_antigo)
    categorias = Categoria.objects.all()
    context = {'componentes': componentes, 
                'categorias':categorias,
                'tipodecomponentes':tipodecomponente,
                'tipo_especifico':TipoDeComponente.objects.get(id=id),
                'id_antigo_antigo':id_antigo,
                'id_antigo':id}
    return render(request, 'core/componentes.html', context)

def modificar(request, id_antigo_antigo, id_antigo, id):
    conta_componentes()
    componente_especifico = Componente.objects.get(id=id)
    tipo_especifico = TipoDeComponente.objects.get(id=id_antigo)
    categorias = Categoria.objects.all()
    form = ComponenteForm(instance=componente_especifico)
    if request.method == "POST":
        form = ComponenteForm(request.POST, instance=componente_especifico)
        if form.is_valid():
            form.save()
            return redirect('../../' + str(id_antigo_antigo) + '/' + str(id_antigo))
    context = {'form': form,
                'componente_especifico': componente_especifico,
                'tipo_especifico': tipo_especifico,
                'categorias':categorias,}
    return render(request, 'core/modificar.html', context)

# def IncreaseCounter(request, id_antigo_antigo, id_antigo):
#     conta_componentes()
#     componentes = Componente.objects.filter(tipo=id_antigo)
#     escolha = Componente.objects.get(id=id)
#     escolha.quantitade = escolha.quantidade + 1
#     escolha.save()



