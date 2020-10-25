def validate(project):
  if project.is_selected('cli'):
    project.error('freertos cannot yet be used with cli', project.target_for_project())
  if project.is_selected('iostream'):
    project.error('freertos cannot yet be used with iostream', project.target_for_project())
