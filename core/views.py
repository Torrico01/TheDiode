from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.forms import UserCreationForm
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django.http import JsonResponse, HttpResponse, HttpResponseRedirect
from django.urls import reverse
from datetime import datetime
from django.apps import apps

from core.config_variables import *
from components.models import *
from projects.models import *
from .forms import *

import json, time, math

esp8266_ip = "192.168.15.17"
delayForSliderAnimation = 0.7

def visitor_ip_address(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for: ip = x_forwarded_for.split(',')[0]
    else: ip = request.META.get('REMOTE_ADDR')
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


# Defining classes to be used in html by accessing its values
class InterfaceProject( object ):
   def __init__( self, pk, id, name, model, grid_row, grid_col ):
       self.pk = pk # To identify the project in the model
       self.id = id # To identify the project in the interface panel
       self.name = name
       self.model = model
       self.grid_row = grid_row
       self.grid_col = grid_col
    
class InterfaceConnection( object ):
    def __init__( self, id1, id2, start_name, end_name, start_grid_row, start_grid_col, end_grid_row, end_grid_col ):
        self.id1 = id1
        self.id2 = id2
        self.start_name = start_name
        self.end_name = end_name
        # Calculate angle, scale and translation of line
        x_start = (start_grid_col*50)-25
        y_start = (start_grid_row*50)-25
        x_end = (end_grid_col*50)-25
        y_end = (end_grid_row*50)-25
        dist_ver = abs(end_grid_row-start_grid_row)
        dist_hor = abs(end_grid_col-start_grid_col)
        position_y = min(y_start,y_end) + 25*dist_ver
        position_x = min(x_start,x_end) + 25*dist_hor
        #translate_y = str(position_y-25)
        #translate_x = str(position_x-25)
        if(dist_hor == 0): angle = math.radians(90)
        else: angle = math.atan(dist_ver/dist_hor)
        if (y_start > y_end): angle = -angle
        if (x_start > x_end): angle = -angle
        angle = str(angle)
        scale = math.sqrt(dist_ver**2 + dist_hor**2)
        # ----------------------------------------------
        self.angle = angle
        self.scale = scale
        self.translate_x = 25*(end_grid_col-start_grid_col)
        self.translate_y = 25*(end_grid_row-start_grid_row)
        self.start_grid_row = start_grid_row
        self.start_grid_col = start_grid_col
        self.middle_grid_row = (start_grid_row+end_grid_row)//2
        self.middle_grid_col = (start_grid_col+end_grid_col)//2

@csrf_exempt
def home(request):
    ip = visitor_ip_address(request)
    print(ip)

    # === ESP8266 Stuff ===============================
    if request.method == "POST" and ip == esp8266_ip:
        # Returns empty json dictionary
        return JsonResponse({})

    if request.method == "GET" and ip == esp8266_ip:
        # Get current time and update .json file
        horaAtual = datetime.now().strftime("%H%M") 

        # Returns json dictionary
        return JsonResponse({})
    # =================================================

    # === Interface Panel Stuff ===============================
    # Update and create interface project positions/connections in db (received as POST from js)
    if request.method == "POST":
        # Sent by javascript in json format
        # Decode json request body and and serialize
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
        if (jsonLoad['type'] == "delete connection"):
            name = jsonLoad['start_name'] + '-' + jsonLoad['end_name']
            Connection.objects.get(name=name).delete()
        if (jsonLoad['type'] == "delete project"):
            # Get model class of the draggable object
            for name, app in apps.app_configs.items():
                if name in ['projects']:
                    for model in app.get_models():
                        if model.__name__ in [jsonLoad['model']]:
                            db_class = model
            # Update grid row and column of draggable in db
            db_query = db_class.objects.get(name=jsonLoad['name'])
            db_query.grid_row = 0
            db_query.grid_col = 0
            db_query.save()

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
                        interface_projects_position.append(InterfaceProject(pk,
                                                                            id,
                                                                            object.name,
                                                                            model.__name__,
                                                                            object.grid_row,
                                                                            object.grid_col))
    # Send interface connections from db to html
    allConnections = Connection.objects.all()
    interface_connections = []
    for connection in allConnections:
        startObj = connection.start_type.get_object_for_this_type(id=connection.start_object_id)
        endObj = connection.end_type.get_object_for_this_type(id=connection.end_object_id)
        # Get angle, scale and translation of line from class
        id1 = projects_id_in_interface[startObj.name]
        id2 = projects_id_in_interface[endObj.name]
        interface_connections.append(InterfaceConnection(id1, id2, startObj.name, endObj.name, startObj.grid_row, startObj.grid_col, endObj.grid_row, endObj.grid_col))
    # =================================================

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

# --> RGB Frame functions
# -----------------------
def hex_to_rgb(hex):
  return tuple(int(hex[i:i+2], 16) for i in (0, 2, 4))

def rgb_to_hex(r, g, b):
    return '#{:02x}{:02x}{:02x}'.format(r, g, b)

def rgbframe_copy_pattern(request, id, seq_id, patt_id):
    if request.method == "POST":
        form = RGBFrameAddSequenceForm(request.POST) # Same form as new sequence
        L1 = form['L1'].value()
        L2 = form['L2'].value()
        T = form['T'].value()
        Color1 = form['Color1'].value()
        Color2 = form['Color2'].value()
        TupleColor1 = hex_to_rgb(Color1[1:])
        TupleColor2 = hex_to_rgb(Color2[1:])
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Count patterns in sequence
        count = len(full_json[str(seq_id)])
        # Build pattern to sequence
        pattern = {"L1": L1,
                "L2": L2,
                "T": T,
                "ICr": TupleColor1[0],
                "ICg": TupleColor1[1],
                "ICb": TupleColor1[2],
                "TCr": TupleColor2[0],
                "TCg": TupleColor2[1],
                "TCb": TupleColor2[2]}
        # Append to full json
        full_json[str(seq_id)][str(count)] = pattern
        full_json[str(seq_id)]["Total"] = count
        if form.is_valid():
            # Update json value in database
            rgbframe_obj.rgb_strip = full_json
            rgbframe_obj.save()
            # Return http response
            return HttpResponseRedirect('')
    else:
        form = RGBFrameAddSequenceForm()
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Get values from current pattern
        pattern_json = full_json[str(seq_id)][str(patt_id)]
        form.fields["L1"].initial = pattern_json["L1"]
        form.fields["L2"].initial = pattern_json["L2"]
        form.fields["T"].initial = pattern_json["T"]
        form.fields["Color1"].initial = rgb_to_hex(pattern_json["ICr"], pattern_json["ICg"], pattern_json["ICb"])
        form.fields["Color2"].initial = rgb_to_hex(pattern_json["TCr"], pattern_json["TCg"], pattern_json["TCb"])
    context = {'form': form,
               'type': 'copy'}
    return render(request, 'core/rgbframeAddSequence.html', context)

def rgbframe_edit_pattern(request, id, seq_id, patt_id):
    if request.method == "POST":
        form = RGBFrameAddSequenceForm(request.POST) # Same form as new sequence
        L1 = form['L1'].value()
        L2 = form['L2'].value()
        T = form['T'].value()
        Color1 = form['Color1'].value()
        Color2 = form['Color2'].value()
        TupleColor1 = hex_to_rgb(Color1[1:])
        TupleColor2 = hex_to_rgb(Color2[1:])
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Build pattern to sequence
        pattern = {"L1": L1,
                "L2": L2,
                "T": T,
                "ICr": TupleColor1[0],
                "ICg": TupleColor1[1],
                "ICb": TupleColor1[2],
                "TCr": TupleColor2[0],
                "TCg": TupleColor2[1],
                "TCb": TupleColor2[2]}
        # Append to full json
        full_json[str(seq_id)][str(patt_id)] = pattern
        if form.is_valid():
            # Update json value in database
            rgbframe_obj.rgb_strip = full_json
            rgbframe_obj.save()
            # Return http response
            return HttpResponseRedirect('')
    else:
        form = RGBFrameAddSequenceForm()
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Get values from current pattern
        pattern_json = full_json[str(seq_id)][str(patt_id)]
        form.fields["L1"].initial = pattern_json["L1"]
        form.fields["L2"].initial = pattern_json["L2"]
        form.fields["T"].initial = pattern_json["T"]
        form.fields["Color1"].initial = rgb_to_hex(pattern_json["ICr"], pattern_json["ICg"], pattern_json["ICb"])
        form.fields["Color2"].initial = rgb_to_hex(pattern_json["TCr"], pattern_json["TCg"], pattern_json["TCb"])
    context = {'form': form,
               'type': 'edit'}
    return render(request, 'core/rgbframeAddSequence.html', context)

def rgbframe_del_pattern(request, id, seq_id, patt_id):
    if request.method == "POST":
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        if (full_json[str(seq_id)]["Total"] == 1): # Delete pattern and sequence
            del full_json[str(seq_id)]
            # Update sequence numbers on the json
            for seq in range(seq_id, len(full_json)):
                full_json[str(seq)] = full_json.pop(str(seq+1))
        else:
            del full_json[str(seq_id)][str(patt_id)] # Delete pattern
            # Update pattern numbers on the sequence
            for patt in range(patt_id, len(full_json[str(seq_id)])):
                full_json[str(seq_id)][str(patt)] = full_json[str(seq_id)].pop(str(patt+1))
            # Update total
            full_json[str(seq_id)]["Total"] = len(full_json[str(seq_id)])-1        
        # Save updated json
        rgbframe_obj.rgb_strip = full_json
        rgbframe_obj.save()
    # Returns
    context = {'type': 'del'}
    return render(request, 'core/rgbframeDelPattern.html', context)

def rgbframe_add_pattern(request, id, seq_id):
    if request.method == "POST":
        form = RGBFrameAddSequenceForm(request.POST) # Same form as new sequence
        L1 = form['L1'].value()
        L2 = form['L2'].value()
        T = form['T'].value()
        Color1 = form['Color1'].value()
        Color2 = form['Color2'].value()
        TupleColor1 = hex_to_rgb(Color1[1:])
        TupleColor2 = hex_to_rgb(Color2[1:])
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Count patterns in sequence
        count = len(full_json[str(seq_id)])
        # Add pattern to end of sequence
        pattern = {"L1": L1,
                "L2": L2,
                "T": T,
                "ICr": TupleColor1[0],
                "ICg": TupleColor1[1],
                "ICb": TupleColor1[2],
                "TCr": TupleColor2[0],
                "TCg": TupleColor2[1],
                "TCb": TupleColor2[2]}
        # Append to full json
        full_json[str(seq_id)][str(count)] = pattern
        full_json[str(seq_id)]["Total"] = count
        if form.is_valid():
            # Update json value in database
            rgbframe_obj.rgb_strip = full_json
            rgbframe_obj.save()
            # Return http response
            return HttpResponseRedirect('')
    else:
        form = RGBFrameAddSequenceForm()
    context = {'form': form,
               'type': 'pattern'}
    return render(request, 'core/rgbframeAddSequence.html', context)

def rgbframe_edit_sequence(request, id, seq_id):
    if request.method == "POST":
        form = RGBFrameEditSequenceForm(request.POST) # Same form as new sequence
        T = form['T'].value()
        Color1 = form['Color1'].value()
        Color2 = form['Color2'].value()
        TupleColor1 = hex_to_rgb(Color1[1:])
        TupleColor2 = hex_to_rgb(Color2[1:])
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        pattern_json = full_json[str(seq_id)]["1"]
        # Append to full json
        for patt in full_json[str(seq_id)]:
            if (patt != "Total"):
                # Check if values changed
                OldTupleColor1 = (pattern_json["ICr"], pattern_json["ICg"], pattern_json["ICb"])
                OldTupleColor2 = (pattern_json["TCr"], pattern_json["TCg"], pattern_json["TCb"])
                new_TupleColor1 = TupleColor1
                if (TupleColor1 == OldTupleColor1):
                    new_TupleColor1 = (full_json[str(seq_id)][patt]["ICr"], full_json[str(seq_id)][patt]["ICg"], full_json[str(seq_id)][patt]["ICb"])
                new_TupleColor2 = TupleColor2
                if (TupleColor2 == OldTupleColor2):
                    new_TupleColor2 = (full_json[str(seq_id)][patt]["TCr"], full_json[str(seq_id)][patt]["TCg"], full_json[str(seq_id)][patt]["TCb"])
                new_T = T
                if (T == pattern_json["T"]):
                    new_T = full_json[str(seq_id)][patt]["T"]
                # Build pattern to sequence
                pattern = {"L1": full_json[str(seq_id)][patt]["L1"],
                        "L2": full_json[str(seq_id)][patt]["L2"],
                        "T": new_T,
                        "ICr": new_TupleColor1[0],
                        "ICg": new_TupleColor1[1],
                        "ICb": new_TupleColor1[2],
                        "TCr": new_TupleColor2[0],
                        "TCg": new_TupleColor2[1],
                        "TCb": new_TupleColor2[2]}
                full_json[str(seq_id)][patt] = pattern
        if form.is_valid():
            # Update json value in database
            rgbframe_obj.rgb_strip = full_json
            rgbframe_obj.save()
            # Return http response
            return HttpResponseRedirect('')
    else:
        form = RGBFrameEditSequenceForm()
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Get values from current pattern
        pattern_json = full_json[str(seq_id)]["1"]
        form.fields["T"].initial = pattern_json["T"]
        form.fields["Color1"].initial = rgb_to_hex(pattern_json["ICr"], pattern_json["ICg"], pattern_json["ICb"])
        form.fields["Color2"].initial = rgb_to_hex(pattern_json["TCr"], pattern_json["TCg"], pattern_json["TCb"])
    context = {'form': form,
               'type': 'edit all'}
    return render(request, 'core/rgbframeAddSequence.html', context)

def rgbframe_del_sequence(request, id, seq_id):
    if request.method == "POST":
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Delete sequence
        del full_json[str(seq_id)]
        # Update sequence numbers on the json
        for seq in range(seq_id, len(full_json)):
            full_json[str(seq)] = full_json.pop(str(seq+1))
        # Save updated json
        rgbframe_obj.rgb_strip = full_json
        rgbframe_obj.save()
    # Returns
    context = {'type': 'del seq'}
    return render(request, 'core/rgbframeDelPattern.html', context)

def rgbframe_add_sequence(request, id):
    if request.method == "POST":
        form = RGBFrameAddSequenceForm(request.POST)
        L1 = form['L1'].value()
        L2 = form['L2'].value()
        T = form['T'].value()
        Color1 = form['Color1'].value()
        Color2 = form['Color2'].value()
        TupleColor1 = hex_to_rgb(Color1[1:])
        TupleColor2 = hex_to_rgb(Color2[1:])
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Count sequences in json
        count = len(full_json)
        # Add single pattern to new sequence
        pattern = {"Total": 1}
        pattern["1"] = {"L1": L1,
                        "L2": L2,
                        "T": T,
                        "ICr": TupleColor1[0],
                        "ICg": TupleColor1[1],
                        "ICb": TupleColor1[2],
                        "TCr": TupleColor2[0],
                        "TCg": TupleColor2[1],
                        "TCb": TupleColor2[2]}
        # Append to full json
        full_json[str(count)] = pattern
        if form.is_valid():
            # Update json value in database
            rgbframe_obj.rgb_strip = full_json
            rgbframe_obj.save()
            # Return http response
            return HttpResponseRedirect('')
    else:
        form = RGBFrameAddSequenceForm()
    context = {'form': form,
               'type': 'sequence'}
    return render(request, 'core/rgbframeAddSequence.html', context)

def rgbframe_copy_sequence(request, id):
    if request.method == "POST":
        # Get json from RGB Frame project
        rgbframe_obj = RGBFrame.objects.get(id=id)
        full_json = rgbframe_obj.rgb_strip
        # Get last sequence and add to end
        last_sequence = full_json[str(len(full_json)-1)]
        full_json[str(len(full_json))] = last_sequence
        print(last_sequence)
        # Save updated json
        rgbframe_obj.rgb_strip = full_json
        rgbframe_obj.save()

    context = {'type': 'copy'}
    return render(request, 'core/rgbframeDelPattern.html', context)

def rgbframe_explore(request, id):
    rgbframe_obj = RGBFrame.objects.get(id=id)
    full_json = rgbframe_obj.rgb_strip
    sequences_array = []
    rules_array = []
    for sequence_num in full_json:
        sequence = full_json[sequence_num]
        pattern_array = []
        for pattern_num in sequence:
            if pattern_num != 'Total':
                pattern = full_json[sequence_num][pattern_num]
                pattern_array.append(RGBFramePattern(pattern_num,
                                                     pattern['L1'],
                                                     pattern['L2'],
                                                     pattern['T'],
                                                     pattern['ICr'],
                                                     pattern['ICg'],
                                                     pattern['ICb'],
                                                     pattern['TCr'],
                                                     pattern['TCg'],
                                                     pattern['TCb']))
        sequences_array.append(RGBFrameSequence(sequence_num,
                                                pattern_array))
    context = {'sequences': sequences_array,
               'rules': rules_array,
               'id': id}
    return render(request, 'core/rgbframeExplore.html', context)

def rgbframe(request):
    rgbframe_all = RGBFrame.objects.all()
    context = {'rgbframe_all': rgbframe_all}
    return render(request, 'core/rgbframe.html', context)

class RGBFrameSequence( object ):
   def __init__( self, num, pattern_array ):
       self.num = num
       self.patterns = pattern_array
       
class RGBFramePattern( object ):
   def __init__( self, num, L1, L2, T, ICr, ICg, ICb, TCr, TCg, TCb ):
       self.num = num
       self.L1 = L1
       self.L2 = L2
       self.T = T
       self.ICr = ICr
       self.ICg = ICg
       self.ICb = ICb
       self.TCr = TCr
       self.TCg = TCg
       self.TCb = TCb