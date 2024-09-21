// interfaces.ts
export interface AssistantWorkspace {
  assistant_id: number;
  created_at: string;
  updated_at: string;
  user_id: string;
  workspace_id: number;
}

export interface Assistant {
  id: number;
  context_length: number;
  created_at: string;
  description: string;
  embeddings_provider: string;
  folder_id: number;
  image_path: string;
  include_profile_context: boolean;
  include_workspace_instructions: boolean;
  model: string;
  name: string;
  prompt: string;
  sharing: string;
  temperature: number;
  updated_at: string;
  user_id: string;
}

export interface Workspace {
  id: string;
  name: string;
  assistants: Assistant[];
}

export interface Chat {
  assistant_id: number;
  context_length: number;
  created_at: string;
  description: string;
  embeddings_provider: string;
  folder_id: number;
  id: number;
  include_profile_context: boolean;
  include_workspace_instructions: boolean;
  model: string;
  name: string;
  prompt: string;
  sharing: string;
  temperature: number;
  updated_at: string;
  user_id: string;
  workspace_id: number;
  // Add any other fields if necessary
}