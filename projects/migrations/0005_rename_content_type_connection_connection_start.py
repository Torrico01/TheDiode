# Generated by Django 4.2 on 2023-06-24 19:33

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('projects', '0004_alter_modularstoragepanel_grid_col_and_more'),
    ]

    operations = [
        migrations.RenameField(
            model_name='connection',
            old_name='content_type',
            new_name='connection_start',
        ),
    ]
