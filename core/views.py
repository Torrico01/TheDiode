from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.forms import UserCreationForm
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django.http import JsonResponse
from django.urls import reverse
import json

from .forms import CriarCategoriaForm, CriarTipoForm, CriarComponenteForm, ComponenteForm
from componente.models import TipoDeComponente, Categoria, Componente

esp8266_ip = "192.168.15.12"

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

def visitor_ip_address(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[0]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip

@csrf_exempt
def home(request):
    ip = visitor_ip_address(request)
    if request.method == "POST" and ip == esp8266_ip:
        # Reads json request data
        s = request.body.decode('utf8')
        in_data = json.loads(s)
        print("In ESP Configuration Data:")
        print(json.dumps(in_data, indent=4, sort_keys=True))

        # Returns empty json dictionary
        return JsonResponse({})

    if request.method == "GET" and ip == esp8266_ip:
        # Reads .json file as dictionary
        f = open("core/static/core/esp.json")
        out_data = json.load(f)
        print("Out ESP Configuration Data:")
        print(json.dumps(out_data, indent=4, sort_keys=True))

        # Returns json dictionary
        return JsonResponse(out_data)

    categorias = Categoria.objects.order_by('-quantidade')
    context = {'categorias':categorias}
    return render(request, 'core/menu.html', context)


def search(request):
    q=request.GET['q']
    tipodecomponentes = TipoDeComponente.objects.filter(nome__icontains=q).order_by('nome')
    componentes = Componente.objects.none()
    if (len(tipodecomponentes) == 0):
        componentes = Componente.objects.filter(nome__icontains=q).order_by('nome')
    else:
        for tipodecomponente in tipodecomponentes:
            componentes |= tipodecomponente.componente_set.all()
    categorias = Categoria.objects.order_by('-quantidade')
    context = {'componentes':componentes, 
               'categorias':categorias, 
               'tipodecomponentes':tipodecomponentes}
    return render(request, 'core/search.html', context)

def tipos_de_componentes(request, id):
    tipodecomponente = TipoDeComponente.objects.filter(categoria=id).order_by('-quantidade')
    categorias = Categoria.objects.order_by('-quantidade')
    context = {'categorias':categorias,
               'tipodecomponentes':tipodecomponente,
               'categoria_especifica':Categoria.objects.get(id=id),
               'id_categoria':id}
    return render(request, 'core/tipos_de_componentes.html', context)

def componentes(request, id_categoria, id):
    componentes = Componente.objects.filter(tipo=id).order_by('nome')
    categorias = Categoria.objects.order_by('-quantidade')
    context = {'categorias':categorias,
               'componentes': componentes,
               'tipo_especifico':TipoDeComponente.objects.get(id=id),
               'id_categoria':id_categoria,
               'id_tipo':id}
    return render(request, 'core/componentes.html', context)

def modificar(request, id_categoria, id_tipo, id):
    componente_especifico = Componente.objects.get(id=id)
    tipo_especifico = TipoDeComponente.objects.get(id=id_tipo)
    form = ComponenteForm(instance=componente_especifico)
    if request.method == "POST":
        form = ComponenteForm(request.POST, instance=componente_especifico)
        if form.is_valid():
            form.save()
            conta_componentes()
            return redirect('../../' + str(id_categoria) + '/' + str(id_tipo))
    context = {'form': form,
               'componente_especifico': componente_especifico,
               'tipo_especifico': tipo_especifico}
    return render(request, 'core/modificar.html', context)

def criarcategoria(request):
    form = CriarCategoriaForm()
    if request.method == "POST":
        form = CriarCategoriaForm(request.POST)
        if form.is_valid():
            form.save()
            return redirect('../')
    context = {'form': form}
    return render(request, 'core/criarcategoria.html', context)

def criartipo(request, id):
    form = CriarTipoForm(id)
    if request.method == "POST":
        form = CriarTipoForm(id,request.POST)
        if form.is_valid():
            form.save()
            return redirect('../../' + str(id))
    context = {'form': form,
               'categoria_especifica':Categoria.objects.get(id=id)}
    return render(request, 'core/criartipo.html', context)

def criarcomponente(request, id_categoria, id):
    form = CriarComponenteForm(id)
    tipo_especifico = TipoDeComponente.objects.get(id=id)
    if request.method == "POST":
        form = CriarComponenteForm(id,request.POST)
        if form.is_valid():
            form.save()
            conta_componentes()
            return redirect('../../../' + str(tipo_especifico.categoria.id) + '/' + str(id))
    context = {'form': form,
               'tipo_especifico':tipo_especifico}
    return render(request, 'core/criarcomponente.html', context)


