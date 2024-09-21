// assistants.ts
import api from './api';
import { Assistant, AssistantWorkspace, Workspace } from './interfaces';

export const getAssistantWorkspacesByWorkspaceId = async (
  workspaceId: string
): Promise<Assistant[]> => {
  // Fetch assistant-workspace relationships
  const response = await api.get<{ assistant_id: number }[]>(
    `/assistant_workspace/${workspaceId}`
  );
  const assistantWorkspaces = response.data;

  // Fetch details of each assistant
  const assistantPromises = assistantWorkspaces.map(async (aw) => {
    const assistantResponse = await api.get<Assistant>(`/assistant/${aw.assistant_id}`);
    return assistantResponse.data;
  });

  const assistants = await Promise.all(assistantPromises);
  return assistants;
};
