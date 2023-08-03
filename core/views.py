from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.forms import UserCreationForm
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django.http import JsonResponse, HttpResponse
from django.urls import reverse
from datetime import datetime
from django.apps import apps

from core.config_variables import *
from components.models import *
from projects.models import *
from .forms import *

import json, time, math

# When creating a connection between two projects
#model1 = ModularStoragePanelBase.objects.first()
#Connection.objects.create(target=model1)
#model2 = ModularStoragePanel.objects.first()
#Connection.objects.create(target=model2)

# When getting the connected elements

#Connection.

esp8266_ip = "192.168.43.21"
delayForSliderAnimation = 0.7

def visitor_ip_address(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[0]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip 

def conta_componentes():
    categorias = Category.objects.all()
    count_total = 0
    count = 0

    for categoria in categorias:
        tipodecomponentes_filtrados = categoria.componenttype_set.all()

        for tipodecomponente in tipodecomponentes_filtrados:
            componentes_filtrados = tipodecomponente.component_set.all()

            for componente_filtrado in componentes_filtrados:
                count += componente_filtrado.quantidade

            tipodecomponente.quantidade = count
            tipodecomponente.save()
            count_total += count
            count = 0
            
        categoria.quantidade = count_total
        categoria.save()
        count_total = 0

def update_config():
    jsonFile = open(PAINEL_CONFIG_JSON)
    dictEspConfig = json.load(jsonFile)

    for categoria in dictEspConfig:
        if categoria != "Funcionalidades":
            for componente in dictEspConfig[categoria]:
                try:
                    componente_id = dictEspConfig[categoria][componente]["ID"].split("/")[2]
                    componente_obj = Component.objects.get(id=int(componente_id))
                    dictEspConfig[categoria][componente]["Quantidade"] = componente_obj.quantidade
                except:
                    continue

    jsonEspConfig = json.dumps(dictEspConfig, indent=4, sort_keys=True)
    with open(PAINEL_CONFIG_JSON, "w") as jsonFile:
        jsonFile.write(jsonEspConfig)
                

def painelDeArmazenamentoModular_json():
    dictJson = {}

    # Get pannels names
    for painel in ModularStoragePanel.objects.all():
        dictJsonPainel = {}
        # Get components in pannels
        for i in range(1,10):
            if i==1: 
                keyName = painel.slot_1
                idx = painel.slot_1.id
            if i==2: 
                keyName = painel.slot_2
                idx = painel.slot_2.id
            if i==3: 
                keyName = painel.slot_3
                idx = painel.slot_3.id
            if i==4: 
                keyName = painel.slot_4
                idx = painel.slot_4.id
            if i==5: 
                keyName = painel.slot_5
                idx = painel.slot_5.id
            if i==6: 
                keyName = painel.slot_6
                idx = painel.slot_6.id
            if i==7: 
                keyName = painel.slot_7
                idx = painel.slot_7.id
            if i==8: 
                keyName = painel.slot_8
                idx = painel.slot_8.id
            if i==9: 
                keyName = painel.slot_9
                idx = painel.slot_9.id
            lim = keyName.limite
            qtd = keyName.quantidade
            keyName = str(i) + ". " + keyName.name
            # Get slots proprieties
            dictJsonPainel.update({keyName:{"ID":idx,"Lim":lim,"Qtd":qtd}})
        dictJson.update(dictJsonPainel)
        
    # Define 'Funcionalidade' values
    dictJson["Funcionalidade"] = {}

    return(dictJson)
    
def painelDeArmazenamentoModular_splitIds(request, cookie_name):
    criar_painel_ids = request.COOKIES.get(cookie_name)
    if (criar_painel_ids != None): criar_painel_num_ids = len(criar_painel_ids.split(","))
    else: criar_painel_num_ids = 0
    return criar_painel_num_ids

def painelDeArmazenamentoModular_contextMsg(request):
    # Conta ids selecionados
    criar_painel_num_ids = painelDeArmazenamentoModular_splitIds(request, "criar_painel_ids")
    # Escreve mensagem da criação do painel
    if request.COOKIES.get('criar_painel') == "True" and criar_painel_num_ids == 9:
        criar_painel_msg = "todos os componentes selecionados"
    elif request.COOKIES.get('criar_painel') == "True":
        criar_painel_msg = str(criar_painel_num_ids) + " componentes selecionados"
    else: 
        criar_painel_msg = 0
    return criar_painel_msg

# Defining classes to be used in html by accessing its values
class InterfaceProject( object ):
   def __init__( self, pk, id, name, model, grid_row, grid_col ):
       self.pk = pk
       self.id = id
       self.name = name
       self.model = model
       self.grid_row = grid_row
       self.grid_col = grid_col
    
class InterfaceConnection( object ):
    def __init__( self, id1, id2, start_grid_row, start_grid_col, end_grid_row, end_grid_col ):
        self.id1 = id1
        self.id2 = id2
        # Calculate angle, scale and translation of line
        x_start = (start_grid_col*50)-25
        y_start = (start_grid_row*50)-25
        x_end = (end_grid_col*50)-25
        y_end = (end_grid_row*50)-25
        dist_ver = abs(end_grid_row-start_grid_row)
        dist_hor = abs(end_grid_col-start_grid_col)
        position_y = min(y_start,y_end) + 25*dist_ver
        position_x = min(x_start,x_end) + 25*dist_hor
        translate_y = str(position_y-25)
        translate_x = str(position_x-25)
        angle = math.atan(dist_ver/dist_hor)
        if (y_start > y_end or x_start > x_end): angle = str(-angle)
        scale = math.sqrt(dist_ver**2 + dist_hor**2)
        # ----------------------------------------------
        self.angle = angle
        self.scale = scale
        self.translate_x = translate_x
        self.translate_y = translate_y


@csrf_exempt
def home(request):
    ip = visitor_ip_address(request)
    print(ip)
    #update_config()

    jsonDict = painelDeArmazenamentoModular_json()
    #print(jsonDict)

    if request.method == "POST" and ip == esp8266_ip:
        # Read json request data
        s = request.body.decode('utf8')
        print(s)
        '''
        in_data = json.loads(s)
        jsonEspConfig = json.dumps(in_data, indent=4, sort_keys=True)
        with open(PAINEL_CONFIG_JSON, "w") as jsonFile:
            jsonFile.write(jsonEspConfig)

        print("In ESP Configuration Data:")
        print(jsonEspConfig)
        '''

        # Returns empty json dictionary
        return JsonResponse({})

    if request.method == "GET" and ip == esp8266_ip:
        # MANDAR PARA O ESP O MODULO ATUAL DO DB, O PEDAÇO DO JSON CORRESPONDENTE, E O RESTANTE DAS INFOS

        # Read .json file as dictionary
        jsonFile = open(PAINEL_CONFIG_JSON)
        dictEspConfig = json.load(jsonFile)

        # Get current time and update .json file
        horaAtual = datetime.now().strftime("%H%M") 
        dictEspConfig["Funcionalidades"]["Horario"]["Atual"] = horaAtual
        jsonEspConfig = json.dumps(dictEspConfig, indent=4, sort_keys=True)
        with open(PAINEL_CONFIG_JSON, "w") as jsonFile:
            jsonFile.write(jsonEspConfig)

        #print("Out ESP Configuration Data:")
        #print(jsonEspConfig)
        #out_hora = {"hora":datetime.now().strftime("%H%M")}

        # Returns json dictionary
        print(dictEspConfig["Capacitor eletrolitico 1"])
        return JsonResponse(dictEspConfig["Capacitor eletrolitico 1"])

    # Update and create interface project positions/connections in db (received as POST from js)
    if request.method == "POST":
        # Request body is of type json -> Decode it and serialize
        jsonLoad = json.loads(request.body.decode('utf8'))

        if (jsonLoad['type'] == "position"):
            # Get model class of the draggable object
            for name, app in apps.app_configs.items():
                if name in ['projects']:
                    for model in app.get_models():
                        if model.__name__ in [jsonLoad['model']]:
                            db_class = model
            # Update grid row and column of draggable in db
            db_query = db_class.objects.get(name=jsonLoad['name'])
            db_query.grid_row = jsonLoad['grid_row']
            db_query.grid_col = jsonLoad['grid_col']
            db_query.save()
        if (jsonLoad['type'] == "connections"):
            name = jsonLoad['start_name'] + '-' + jsonLoad['end_name']
            start_type = ContentType.objects.filter(model__icontains=jsonLoad['start_type']).first()
            end_type = ContentType.objects.filter(model__icontains=jsonLoad['end_type']).first()
            Connection.objects.create(name=name,
                                      start_type=start_type,
                                      start_object_id=jsonLoad['start_obj_id'],
                                      end_type=end_type,
                                      end_object_id=jsonLoad['end_obj_id'])
    # Send interface projects position from db to html
    interface_projects_position = []
    projects_id_in_interface = {}
    count = 0
    for name, app in apps.app_configs.items():
        if name in ['projects']:
            for model in app.get_models():
                if not model.__name__ in ['Connection']:
                    for object in model.objects.all():
                        count += 1
                        pk = object.id
                        id = "p" + str(count)
                        projects_id_in_interface[object.name] = id
                        interface_projects_position.append(InterfaceProject(pk, id, object.name, model.__name__, object.grid_row, object.grid_col))
    # Send interface connections from db to html
    allConnections = Connection.objects.all()
    interface_connections = []
    for connection in allConnections:
        startObj = connection.start_type.get_object_for_this_type(id=connection.start_object_id)
        endObj = connection.end_type.get_object_for_this_type(id=connection.end_object_id)
        # Get angle, scale and translation of line from class
        id1 = projects_id_in_interface[startObj.name]
        id2 = projects_id_in_interface[endObj.name]
        interface_connections.append(InterfaceConnection(id1, id2, startObj.grid_row, startObj.grid_col, endObj.grid_row, endObj.grid_col))

    context = {'interface_projects_position': interface_projects_position,
               'interface_connections': interface_connections}
    response = render(request, 'core/home.html', context)
    return response

def categorias(request):
    # Lista os componentes
    categorias = Category.objects.order_by('-quantidade')
    # (Se) Criando Painel de Armazenamento Modular:
    criar_painel_msg = painelDeArmazenamentoModular_contextMsg(request)
    # Return render
    context = {'categorias':categorias,
               'criar_painel_msg':criar_painel_msg}
    response = render(request, 'core/categorias.html', context)
    return response

def search(request):
    time.sleep(delayForSliderAnimation) # Wait for animation to finish
    q=request.GET['q']
    tipodecomponentes = ComponentType.objects.filter(name__icontains=q).order_by('name')
    componentes = Component.objects.none()
    if (len(tipodecomponentes) == 0):
        componentes = Component.objects.filter(name__icontains=q).order_by('name')
    else:
        for tipodecomponente in tipodecomponentes:
            componentes |= tipodecomponente.component_set.all()
    categorias = Category.objects.order_by('-quantidade')
    context = {'componentes':componentes, 
               'categorias':categorias, 
               'tipodecomponentes':tipodecomponentes}
    return render(request, 'core/search.html', context)

def tiposdecomponentes(request, id):
    # Lista os componentes
    tipodecomponente = ComponentType.objects.filter(categoria=id).order_by('-quantidade')
    categorias = Category.objects.order_by('-quantidade')
    # (Se) Criando Painel de Armazenamento Modular:
    criar_painel_msg = painelDeArmazenamentoModular_contextMsg(request)
    # Return render
    context = {'categorias':categorias,
               'tipodecomponentes':tipodecomponente,
               'categoria_especifica':Category.objects.get(id=id),
               'id_categoria':id,
               'criar_painel_msg':criar_painel_msg}
    return render(request, 'core/tiposdecomponentes.html', context)

def componentes(request, id_categoria, id):
    # Lista os componentes
    componentes = Component.objects.filter(tipo=id).order_by('name')
    categorias = Category.objects.order_by('-quantidade')
    # (Se) Criando Painel de Armazenamento Modular:
    criar_painel_msg = painelDeArmazenamentoModular_contextMsg(request)
    # Return render
    context = {'categorias':categorias,
               'componentes': componentes,
               'tipo_especifico':ComponentType.objects.get(id=id),
               'id_categoria':id_categoria,
               'id_tipo':id,
               'criar_painel_msg':criar_painel_msg}
    return render(request, 'core/componentes.html', context)

def modificar(request, id_categoria, id_tipo, id):
    componente_especifico = Component.objects.get(id=id)
    tipo_especifico = ComponentType.objects.get(id=id_tipo)
    form = ComponenteForm(instance=componente_especifico)
    if request.method == "POST":
        form = ComponenteForm(request.POST, instance=componente_especifico)
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            conta_componentes()
            update_config()
            return redirect("./")
    context = {'form': form,
               'componente_especifico': componente_especifico,
               'tipo_especifico': tipo_especifico}
    return render(request, 'core/modificar.html', context)

def criarcategoria(request):
    form = CriarCategoriaForm()
    if request.method == "POST":
        form = CriarCategoriaForm(request.POST)
        print(list(request.POST.items()))
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            return redirect('../')
    context = {'form': form}
    return render(request, 'core/criarcategoria.html', context)

def criartipo(request, id):
    form = CriarTipoForm(id)
    if request.method == "POST":
        form = CriarTipoForm(id,request.POST)
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            return redirect('../')
    context = {'form': form,
               'categoria_especifica':Category.objects.get(id=id)}
    return render(request, 'core/criartipo.html', context)

def criarcomponente(request, id_categoria, id):
    form = CriarComponenteForm(id)
    tipo_especifico = ComponentType.objects.get(id=id)
    if request.method == "POST":
        form = CriarComponenteForm(id,request.POST)
        if form.is_valid():
            time.sleep(delayForSliderAnimation) # Wait for animation to finish
            form.save()
            conta_componentes()
            return redirect('../')
    context = {'form': form,
               'tipo_especifico':tipo_especifico}
    return render(request, 'core/criarcomponente.html', context)


def paineldearmazenamentomodular(request):
    criar_painel_num_ids = painelDeArmazenamentoModular_splitIds(request, "criar_painel_ids")
    if request.COOKIES.get('criar_painel') == "True" and criar_painel_num_ids == 9:
        criar_painel_completo = "True"
    else:
        criar_painel_completo = "False"
    context = {"criar_painel_completo":criar_painel_completo}
    return render(request, 'core/painelDeArmazenamentoMod.html', context)

def criarpaineldearmazenamentomodular(request):
    criar_painel_num_ids = painelDeArmazenamentoModular_splitIds(request, "criar_painel_ids")
    painel_component_selection_completed = request.COOKIES.get('criar_painel') == "True" and criar_painel_num_ids == 9
    if painel_component_selection_completed:
        form = CriarPainelForm(request.COOKIES.get("criar_painel_ids"))
        if request.method == "POST":
            form = CriarPainelForm(request.COOKIES.get("criar_painel_ids"),request.POST)
            if form.is_valid():
                form.save()
                if painel_component_selection_completed: criar_painel_completo = "True"
                else: criar_painel_completo = "False"
                context = {"criar_painel_completo":criar_painel_completo}
                response = render(request, 'core/painelDeArmazenamentoMod.html', context)
                response.set_cookie('criar_painel', "False") # Set cookie false
                response.delete_cookie('criar_painel_ids')
                return response
        context = {'form': form}
        response = render(request, 'core/criarPainelDeArmazenamentoMod.html', context)
    else:
        if painel_component_selection_completed: criar_painel_completo = "True"
        else: criar_painel_completo = "False"
        context = {"criar_painel_completo":criar_painel_completo}
        response = render(request, 'core/painelDeArmazenamentoMod.html', context)
        response.set_cookie('criar_painel', "True") # Set cookie true
    return response