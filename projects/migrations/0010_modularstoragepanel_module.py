# Generated by Django 4.2 on 2023-08-13 14:54

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('projects', '0009_rename_nome_connection_name_and_more'),
    ]

    operations = [
        migrations.AddField(
            model_name='modularstoragepanel',
            name='module',
            field=models.PositiveIntegerField(default=0, verbose_name='Module selected'),
        ),
    ]