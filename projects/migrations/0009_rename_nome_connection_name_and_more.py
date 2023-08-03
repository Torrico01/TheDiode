# Generated by Django 4.2 on 2023-06-24 20:11

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('projects', '0008_connection_end_object_id_connection_end_type_and_more'),
    ]

    operations = [
        migrations.RenameField(
            model_name='connection',
            old_name='nome',
            new_name='name',
        ),
        migrations.RenameField(
            model_name='modularstoragepanel',
            old_name='display_de_7_segmentos',
            new_name='display_7_segment',
        ),
        migrations.RenameField(
            model_name='modularstoragepanel',
            old_name='matriz_de_leds',
            new_name='leds_matrix',
        ),
        migrations.RenameField(
            model_name='modularstoragepanel',
            old_name='nome',
            new_name='name',
        ),
        migrations.RenameField(
            model_name='modularstoragepanelbase',
            old_name='nome',
            new_name='name',
        ),
        migrations.RenameField(
            model_name='modularstoragepanelbase',
            old_name='chave_rotativa',
            new_name='rotary_switch',
        ),
    ]
